//Inclusion of basic windows header files and direct3D header files
#include "renderer.h"
//DXTK headers
#include "SimpleMath.h"

//System
#include <windows.h>

//Self-Made Headers
#include "GameData.h"
#include "DrawData.h"
#include "GOList.h"
Renderer::Renderer(ID3D11Device * _pd3dDevice, HWND _hWnd, HINSTANCE _hInstance)
{
	//Input controller
	hWnd = _hWnd;
	DiKeyboard = nullptr;
	DiMouse = nullptr;
	DirectInput = nullptr;
	//Initialised Input
	InitDirectInput(_hInstance);

	//Initialises data into the game state and game data being shared between all game objects
	InitGameData();

	//InitVBGO --Come back to--
	VBGO::Init(_pd3dDevice);
	//Window Aspect Ratio helper
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	float AR = (float)width / (float)height;

	//Make Camera
	m_cam = new Camera(0.25f * XM_PI, AR, 1.0f, 10000.0f, Vector3::UnitY, Vector3::Zero);
	m_cam->setPos(Vector3(0.0f, 100.0f, 100.0f));
	m_GameObjects.push_back(m_cam);
	//make Light
	m_light = new Light(Vector3(0.0f, 100.0f, 160.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.4f, 0.1f, 0.1f, 1.0f));
	m_GameObjects.push_back(m_light);
	//Create draw data
	InitDrawData();
	//Do marching cube stuff
	MarchingCubes* VBMC = new MarchingCubes();
	VBMC->init(Vector3(-8.0f, -8.0f, -17.0f), Vector3(8.0f, 8.0f, 23.0f), 60.0f*Vector3::One, 0.01, _pd3dDevice);
	VBMC->setPos(Vector3(100, 0, -100));
	VBMC->setPitch(-XM_PIDIV2);
	VBMC->setScale(Vector3(3, 3, 1.5));
	m_GameObjects.push_back(VBMC);
}

Renderer::~Renderer()
{
	//Delete data
	delete m_GD;
	delete m_DD;

	//Tidy up VBGO

	//Release direct input
	if (DiKeyboard)
	{
		DiKeyboard->Unacquire();
		DiKeyboard->Release();
	}
	if (DiMouse)
	{
		DiMouse->Unacquire();
		DiMouse->Release();
	}
	if (DirectInput)
	{
		DirectInput->Release();
	}

	//Delete all gameobjects
	for (std::list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		delete (*it);
	}
	m_GameObjects.clear();
}

bool Renderer::Tick()
{
	DetectInput();

	RECT window;
	GetWindowRect(hWnd, &window);
	SetCursorPos((window.left + window.right) >> 1, (window.bottom + window.top) >> 1);

	//Calculate Delta Time
	DWORD currentTime = GetTickCount();
	m_GD->m_dt = min((float)(currentTime - play_time) / 1000.0f, 0.1f);
	play_time = currentTime;


	//State System Implementation

	switch (m_GD->m_GS)
	{
	case GS_MAIN_CAM:
		PlayTick();
	}
	return true;
}
void Renderer::PlayTick()
{
	//Update all objects
	for (std::list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Tick(m_GD);
	}
}

void Renderer::Draw(ID3D11DeviceContext * _pd3dImmediateContext)
{
	m_DD->m_pd3dImmediateContext = _pd3dImmediateContext;

	//VBGO update constant buffer m_DD

	//draw all objects
	for (std::list<GameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Draw(m_DD);
	}
}


void Renderer::InitD3D(HWND hWnd) //The creation of Direct3D to use it
{
	//create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;
	//Clears out the struct for us
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	//fill the swap chain description struct
	scd.BufferCount = 1;	//One Back Buffer
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; //Use 32-bit colours
	scd.BufferDesc.Width = 100; //Set the back buffer width
	scd.BufferDesc.Height = 100; //Set the back buffer height
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //How swap chain is to be used
	scd.OutputWindow = hWnd; // The window to be used
	scd.SampleDesc.Count = 4; // How many multi samples
	scd.Windowed = TRUE; // If the window is full-screen/Windowed
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //Allow full screen switching
	// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &scd, &swapchain, &dev, NULL, &devcon);

	//get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	//use the back buffer address to create the render target
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	//set the render target as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, NULL);

	//Rasterizer setup
	ID3D11RasterizerState *pRasterizerState;

	D3D11_RASTERIZER_DESC Rast;

	Rast.FillMode = D3D11_FILL_SOLID;
	Rast.CullMode = D3D11_CULL_NONE;

	dev->CreateRasterizerState(&Rast, &pRasterizerState);

	//Set the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 100;
	viewport.Height = 100;
	devcon->RSSetViewports(1, &viewport); //Activates viewport structs
	InitPipeline();
	InitGraphics();
}

void Renderer::CleanD3D() //This cleans up Direct3D and COM
{
	swapchain->SetFullscreenState(FALSE, NULL); //Switch to windowed mode
	PostMessage(hWnd, WM_DESTROY, 0, 0);
	// close and release all existing COM objects
	pLayout->Release();
	pVS->Release();
	pPS->Release();
	pVBuffer->Release();
	pIBuffer->Release();
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
	cbPerObjectBuffer->Release();
	DiKeyboard->Unacquire();
	DiMouse->Unacquire();
	DirectInput->Release();
}
void Renderer::RenderFrame(double time) //renders a single frame
{
	//clears the back buffer to a deep blue
	//devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f)); //Fills the render target with a specific colour

	////select which vertex buffer to display
	UINT stride = 10;
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(pIBuffer, DXGI_FORMAT_R16_UINT, 0);
	//Set the world projection matrix for the camera
	World = DirectX::XMMatrixIdentity();

	WVP = World * camView * camProjection;

	cbPerObj.WVP = DirectX::XMMatrixTranspose(WVP);

	devcon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);

	devcon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	//select which primative type we are using
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//draw the vertex bufer to the back buffer
	DetectInput();
	//devcon->Draw(8, 0);
	devcon->DrawIndexed(100, 0, 0);
	//switch te back buffer and front buffer
	swapchain->Present(0, 0); // this means everything drawn in the back buffer appears
}
void Renderer::InitPipeline()
{
//	//Load and compile the two shaders
//	ID3D10Blob *VS, *PS;
 D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
//	//D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);
//
//	// encapsulate both shaders into shader obejcts
//	//dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
//	//dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);
//
//	//Set the shader objects
//	devcon->VSSetShader(pVS, 0, 0);
//	devcon->PSSetShader(pPS, 0, 0);
//	//create the input layout object
//	D3D11_INPUT_ELEMENT_DESC ied[] =
//	{
//		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
//	};
//
//	dev->CreateInputLayout(ied, 3, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
//	devcon->IASetInputLayout(pLayout);
//
//	D3D11_BUFFER_DESC cbbd;
//	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
//
//	cbbd.Usage = D3D11_USAGE_DEFAULT;
//	cbbd.ByteWidth = sizeof(cbPerObject);
//	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cbbd.CPUAccessFlags = 0;
//	cbbd.MiscFlags = 0;
//	dev->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);
//	//Camera information
//	camPosition = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
//	camTarget = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
//	camUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
//	//Sets the view matrix
//	camView = DirectX::XMMatrixLookAtLH(camPosition, camTarget, camUp);
//	//Sets the projection matrix
//	camProjection = DirectX::XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);
}
void Renderer::InitGraphics()
{
	float length = 1.0f;
	float height = 1.0f;
	float width = 1.0f;

	////create Two cube using the VERTEX struct
	//VERTEX Vertices[] =
	//{
	//	{ 0.0f, 0.0f, 0.0f, -length, height, -width , D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
	//	{ 0.0f, 0.0f, 0.0f, length, height, -width, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
	//	{ 0.0f, 0.0f, 0.0f, -length, -height, -width, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
	//	{ 0.0f, 0.0f, 0.0f, length, -height, -width, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f) },

	//	{ 0.0f, 0.0f, 0.0f, -length, height, width , D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f) },
	//	{ 0.0f, 0.0f, 0.0f, length, height, width, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f) },
	//	{ 0.0f, 0.0f, 0.0f, -length, -height, width, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f) },
	//	{ 0.0f, 0.0f, 0.0f, length, -height, width, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) },

	//	{ 7.0f, 0.0f, 0.0f, -length, height, -width , D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) }, //Second cube vertices
	//	{ 7.0f, 0.0f, 0.0f, length, height, -width, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },	//function can be written to create multiple
	//	{ 7.0f, 0.0f, 0.0f, -length, -height, -width, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) }, //Using normal allows me to change that to move the cube
	//	{ 7.0f, 0.0f, 0.0f, length, -height, -width, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f) },

	//	{ 7.0f, 0.0f, 0.0f, -length, height, width , D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f) },
	//	{ 7.0f, 0.0f, 0.0f, length, height, width, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f) },
	//	{ 7.0f, 0.0f, 0.0f, -length, -height, width, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f) },
	//	{ 7.0f, 0.0f, 0.0f, length, -height, width, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) },
	//};
	//create the vertex buffer
	//D3D11_BUFFER_DESC bd;
	//ZeroMemory(&bd, sizeof(bd));

	//bd.Usage = D3D11_USAGE_DYNAMIC; //write access by CPU and GPU
	//bd.ByteWidth = sizeof(VERTEX) * 16; // Size is the VERTEX struct * - change depending on vertex contained
	//bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Use as a vertex buffer
	//bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE; //Allow CPU to write to buffer

	//dev->CreateBuffer(&bd, NULL, &pVBuffer); //create the buffer

	////copy the vertices into the buffer
	//D3D11_MAPPED_SUBRESOURCE ms;
	//devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); //Map the buffer
	//memcpy(ms.pData, &Vertices, sizeof(Vertices)); //Copy the data
	//devcon->Unmap(pVBuffer, NULL);

	//Indices array
	short indices[] =
	{
		0, 1, 2, //side list
		2, 1, 3,
		4, 0, 6,
		6, 0, 2,
		7, 5, 6,
		6, 5, 4,
		3, 1, 7,
		7, 1, 5,
		4, 5, 0,
		0, 5, 1,
		3, 7, 2,
		2, 7, 6,

		8, 9, 10, //side list //To render more than one cube, a new index list needs to be made
		10, 9, 11,	//New list is previous list + 8
		12, 8, 14,	//Could write a function to handle this
		14, 8, 10,
		15, 13, 14,
		14, 15, 12,
		11, 9, 15,
		15, 9, 13,
		12, 13, 8,
		8, 13, 9,
		11, 15, 10,
		10, 15, 14,

	};
	//create the index buffer
	D3D11_BUFFER_DESC id;
	ZeroMemory(&id, sizeof(id));

	//defines allowed usage
	id.Usage = D3D11_USAGE_DYNAMIC;
	id.ByteWidth = sizeof(indices) * 12;
	id.BindFlags = D3D11_BIND_INDEX_BUFFER;
	id.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	dev->CreateBuffer(&id, NULL, &pIBuffer); // creates the buffer

	D3D11_MAPPED_SUBRESOURCE is;
	devcon->Map(pIBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &is);
	memcpy(is.pData, indices, sizeof(indices));
	devcon->Unmap(pIBuffer, NULL);
}
void Renderer::UpdateCamera()
{
	camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = DirectX::XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = DirectX::XMVector3Normalize(camTarget);

	//Free Look Camera
	CamRight = DirectX::XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	CamForward = DirectX::XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = DirectX::XMVector3Cross(CamForward, CamRight);

	camPosition = DirectX::XMVectorAdd(camPosition, DirectX::XMVectorMultiply(DirectX::XMLoadFloat(&moveLeftRight), CamRight));
	camPosition = DirectX::XMVectorAdd(camPosition, DirectX::XMVectorMultiply(DirectX::XMLoadFloat(&moveBackFoward), CamForward));
	moveLeftRight = 0.0f;
	moveBackFoward = 0.0f;
	camTarget = DirectX::XMVectorAdd(camPosition, camTarget);
	camView = DirectX::XMMatrixLookAtLH(camPosition, camTarget, camUp);
}
bool Renderer::InitDirectInput(HINSTANCE hInstance)
{
	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL);
	hr = DirectInput->CreateDevice(GUID_SysKeyboard, &DiKeyboard, NULL);
	hr = DirectInput->CreateDevice(GUID_SysMouse, &DiMouse, NULL);
	hr = DiKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = DiKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = DiMouse->SetDataFormat(&c_dfDIMouse);
	hr = DiMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	return true;
}
void Renderer::InitGameData()
{
	m_GD = new GameData;
	m_GD->m_keyboardState = m_keyboardState;
	m_GD->m_prevkKeyboardState = m_prevKeyboardState;
	m_GD->m_GS = GS_MAIN_CAM;
	m_GD->m_mouseState = &mouseLastState;



}
void Renderer::InitDrawData()
{
	m_DD = new DrawData;
	m_DD->m_pd3dImmediateContext = nullptr;
	m_DD->m_cam = m_cam;
	m_DD->m_light = m_light;

}
void Renderer::DetectInput()
{
	DIMOUSESTATE mouseCurrState;
	BYTE keyboardState[256];

	DiKeyboard->Acquire();
	DiMouse->Acquire();

	DiMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	DiKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(hWnd, WM_DESTROY, 0, 0);

	float speed = 15.0f * m_GD->m_dt;

	if (keyboardState[DIK_A] & 0x80)
	{
		moveLeftRight -= speed;
	}
	if (keyboardState[DIK_D] & 0x80)
	{
		moveLeftRight += speed;
	}
	if (keyboardState[DIK_W] & 0x80)
	{
		moveBackFoward += speed;
	}
	if (keyboardState[DIK_S] & 0x80)
	{
		moveBackFoward -= speed;
	}
	if ((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		camYaw += mouseLastState.lX * 0.0001f;

		camPitch += mouseCurrState.lY * 0.0001f;

		mouseLastState = mouseCurrState;
	}
	//UpdateCamera();

	return;
}
void Renderer::StartTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;
}
double Renderer::GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - CounterStart) / countsPerSecond;
}
double Renderer::GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
	{
		tickCount = 0.0f;
	}
	
	return float(tickCount) / countsPerSecond;
}


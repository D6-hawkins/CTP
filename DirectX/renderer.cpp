//Inclusion of basic windows header files and direct3D header files
#include "renderer.h"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
//LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


//Input controls
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) //The creation of the window
{
	//The handle for the window, filled by  a function
	//this struct holds information for the window class
	WNDCLASSEX wc;
	//Clears out the wndow class
	Renderer *pRend = new Renderer();
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	//this fills in the struct with necessary information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)Renderer::WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass1";
	//registers the window class
	RegisterClassEx(&wc);

	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }; //Sets the size but not the position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); //Adjust the size

	HWND hWndStore;
	//creates the window and use the result as the handle
	hWndStore = CreateWindowEx(NULL,
		L"WindowClass1", //name of the Window class
		L"AT Project", //Title of the window
		WS_OVERLAPPEDWINDOW, //Window Style
		300,				//X position of the window
		300,				//Y position of the window
		wr.right - wr.left,	//Width of the window
		wr.bottom - wr.top,	//height of the window
		NULL,				//Parent window
		NULL,				//Menu system
		hInstance,			//Application handle
		NULL);				//Used with multiple windows
	
	pRend->sethWnd(hWndStore);
	//displays the window on the screen
	ShowWindow(hWndStore, nCmdShow);

	if (!pRend->InitDirectInput(hInstance))
	{
		MessageBox(0, L"Direct Input Initialization - FAILED", L"ERROR", MB_OK);
		return 0;
	}
	//initialize Direct3D
	pRend->InitD3D(hWndStore);
	//window Main Loop:

	//Struct holds windows event messages
	MSG msg = {0};

	//wait for the next message in the queue, stores the result in msg
	while (TRUE)
	{
		//Checks if any messages are waiting in queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//translates keystroke messages into the right format
			TranslateMessage(&msg);

			//Sends the message to the windowProc function
			DispatchMessage(&msg);

			//checks if it's time to quit
			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
		else
		{
			pRend->frameCount++;
			if (pRend->GetTime() > 1.0f)
			{
				pRend->fps = pRend->frameCount;
				pRend->frameCount = 0;
				pRend->StartTimer();
			}
			pRend->frameTime = pRend->GetFrameTime();
			pRend->RenderFrame(pRend->frameTime);
		}
	}

	//Clear up DirectX and COM
	pRend->CleanD3D();
	//returns this part of the WM_QUIT message to Windows
	return msg.wParam;
}

//message handler function for the program
LRESULT CALLBACK Renderer::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//Sort through and find what code to run for the message given
	switch (message)
	{
		//this message is read when the window is closed
		case WM_DESTROY:
		{
			//Closes the application
			PostQuitMessage(0);
			return 0;
			break;
		}
	}

	//Handles any messages the switch statement didn;t
	return DefWindowProc(hWnd, message, wParam, lParam);
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
	scd.BufferDesc.Width = SCREEN_WIDTH; //Set the back buffer width
	scd.BufferDesc.Height = SCREEN_HEIGHT; //Set the back buffer height
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
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;
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
	devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f)); //Fills the render target with a specific colour

	////select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
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
	DetectInput(time);
	//devcon->Draw(8, 0);
	devcon->DrawIndexed(100, 0, 0);
	//switch te back buffer and front buffer
	swapchain->Present(0, 0); // this means everything drawn in the back buffer appears
}
void Renderer::InitPipeline()
{
	//Load and compile the two shaders
	ID3D10Blob *VS, *PS;
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

	// encapsulate both shaders into shader obejcts
	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	//Set the shader objects
	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);
	//create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	dev->CreateInputLayout(ied, 3, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);

	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;
	dev->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);
	//Camera information
	camPosition = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
	camTarget = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//Sets the view matrix
	camView = DirectX::XMMatrixLookAtLH(camPosition, camTarget, camUp);
	//Sets the projection matrix
	camProjection = DirectX::XMMatrixPerspectiveFovLH(0.4f*3.14f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);
}
void Renderer::InitGraphics()
{
	float length = 1.0f;
	float height = 1.0f;
	float width = 1.0f;

	//create Two cube using the VERTEX struct
	VERTEX Vertices[] =
	{
		{ 0.0f, 0.0f, 0.0f, -length, height, -width , D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
		{ 0.0f, 0.0f, 0.0f, length, height, -width, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ 0.0f, 0.0f, 0.0f, -length, -height, -width, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
		{ 0.0f, 0.0f, 0.0f, length, -height, -width, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f) },

		{ 0.0f, 0.0f, 0.0f, -length, height, width , D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f) },
		{ 0.0f, 0.0f, 0.0f, length, height, width, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f) },
		{ 0.0f, 0.0f, 0.0f, -length, -height, width, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f) },
		{ 0.0f, 0.0f, 0.0f, length, -height, width, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) },

		{ 7.0f, 0.0f, 0.0f, -length, height, -width , D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) }, //Second cube vertices
		{ 7.0f, 0.0f, 0.0f, length, height, -width, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },	//function can be written to create multiple
		{ 7.0f, 0.0f, 0.0f, -length, -height, -width, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) }, //Using normal allows me to change that to move the cube
		{ 7.0f, 0.0f, 0.0f, length, -height, -width, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f) },

		{ 7.0f, 0.0f, 0.0f, -length, height, width , D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f) },
		{ 7.0f, 0.0f, 0.0f, length, height, width, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f) },
		{ 7.0f, 0.0f, 0.0f, -length, -height, width, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f) },
		{ 7.0f, 0.0f, 0.0f, length, -height, width, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) },
	};
	//create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC; //write access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * 16; // Size is the VERTEX struct * - change depending on vertex contained
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Use as a vertex buffer
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE; //Allow CPU to write to buffer

	dev->CreateBuffer(&bd, NULL, &pVBuffer); //create the buffer

	//copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); //Map the buffer
	memcpy(ms.pData, &Vertices, sizeof(Vertices)); //Copy the data
	devcon->Unmap(pVBuffer, NULL);

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
void Renderer::DetectInput(double time)
{
	DIMOUSESTATE mouseCurrState;
	BYTE keyboardState[256];

	DiKeyboard->Acquire();
	DiMouse->Acquire();

	DiMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	DiKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(hWnd, WM_DESTROY, 0, 0);

	float speed = 15.0f * time;

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
	UpdateCamera();

	return;
}
void Renderer::RenderText(std::wstring text, int inInt)
{
	std::wostringstream printString;
	printString << text << inInt;
	printText = printString.str();
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

void Renderer::sethWnd(HWND hwnd)
{
	hWnd = hwnd;
}

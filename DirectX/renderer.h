#pragma once
#include <Windows.h>
#include <dinput.h>
#include "Effects.h"
#include "CommonStates.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "Audio.h"
#include <list>
using namespace DirectX;
class DrawData;
class GameData;
class Camera;
class Light;
class GameObject;
class Renderer
{
public:
	Renderer(ID3D11Device* _pd3dDevice, HWND _hWnd, HINSTANCE _hInstance);
	virtual ~Renderer();
	bool Tick();
	void Draw(ID3D11DeviceContext* _pd3dImmediateContext);
	bool InitDirectInput(HINSTANCE hInstance); //Initialised input system
	void InitGameData();
	void InitDrawData();

	void InitD3D(HWND hWnd);  //Sets up and initializes Direct3D
	void CleanD3D(void);	//closes Direct3D and released the memory
	void RenderFrame(double time); //Renders a single frame
	void InitPipeline(void); //Initialised the shader pipeline
	void InitGraphics(void); //Initialises vertexs
	void UpdateCamera(void); //Camera update function
	
	void DetectInput(); //Detects input
	void RenderText(std::wstring text, int inInt);

	void StartTimer();
	double GetTime();
	double GetFrameTime();

	void sethWnd(HWND hwnd);


	int frameCount = 0;
	int fps = 0;
	double frameTime;
private:
	DWORD play_time; //Tracker for delta time
	Camera* m_cam; //Main camera
	Light* m_light; //Base light
	
	//Input
	std::list <GameObject*> m_GameObjects; //List of gameobjcest
	IDirectInputDevice8 *DiKeyboard; //Keyboard capture
	IDirectInputDevice8 *DiMouse; //Mouse capture
	unsigned char m_keyboardState[256]; 
	unsigned char m_prevKeyboardState[256];
	DIMOUSESTATE mouseLastState; //Mouse last state storage
	LPDIRECTINPUT8 DirectInput;
	//---

	//Data storage
	GameData* m_GD; //Data to be shared to all game objects as they are ticked
	DrawData* m_DD; //data to be shared to all game objects as they are drawn
	//-----

	//Tick function for each state
	void PlayTick();


	HWND hWnd;
	HRESULT hr;	













	

	float rotx = 0;
	float rotz = 0;
	float scaleX = 1.0f;
	float scaleY = 1.0f;

	DirectX::XMMATRIX RotationX;
	DirectX::XMMATRIX RotationY;

	//global declarations
	IDXGISwapChain *swapchain;  //Pointer to the swap chain interface
	ID3D11Device *dev;			//Pointer to the direct3D device interface
			//Pointer to the HRESULT
	ID3D11DeviceContext *devcon; //the pointer to the direct3D device context
	ID3D11RenderTargetView *backbuffer; //Pointer to the render target
	ID3D11VertexShader *pVS; //the vertex shader
	ID3D11PixelShader *pPS; //the pixel shader
	ID3D11Buffer *pVBuffer; // The Vertex Buffer
	ID3D11Buffer *pIBuffer; //Index Buffer
	ID3D11InputLayout *pLayout; //Layout for shaders
	ID3D11Buffer *cbPerObjectBuffer; //Buffer to store constant buffer variables


	DirectX::XMMATRIX WVP;
	DirectX::XMMATRIX World;
	DirectX::XMMATRIX camView;
	DirectX::XMMATRIX camProjection;

	DirectX::XMVECTOR camPosition; //Vector to hold camera position
	DirectX::XMVECTOR camTarget; //Holds camera target
	DirectX::XMVECTOR camUp; //Holds the direction of the camera

	std::wstring printText;
	struct cbPerObject
	{
		DirectX::XMMATRIX WVP;
	};

	cbPerObject cbPerObj;

	DirectX::XMVECTOR DefaultForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR DefaultRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR CamForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR CamRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	DirectX::XMMATRIX camRotationMatrix;
	DirectX::XMMATRIX groundWorld;

	float moveLeftRight = 0.0f;
	float moveBackFoward = 0.0f;

	float camYaw = 0.0f;
	float camPitch = 0.0f;

	double countsPerSecond = 0.0;
	__int64 CounterStart = 0;

	__int64 frameTimeOld = 0;



};
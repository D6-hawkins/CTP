#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <vector>
#include "VoxelManager.h"
//Screen resolution directives
//include Direct3D library files
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")
//Include Direct3D Input files
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#include <dinput.h>
#include <iostream>
#include <sstream>


class Renderer
{
public:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void InitD3D(HWND hWnd);  //Sets up and initializes Direct3D
	void CleanD3D(void);	//closes Direct3D and released the memory
	void RenderFrame(double time); //Renders a single frame
	void InitPipeline(void); //Initialised the shader pipeline
	void InitGraphics(void); //Initialises vertexs
	void UpdateCamera(void); //Camera update function
	bool InitDirectInput(HINSTANCE hInstance); //Initialised input system
	void DetectInput(double time); //Detects input
	void RenderText(std::wstring text, int inInt);

	void StartTimer();
	double GetTime();
	double GetFrameTime();

	void sethWnd(HWND hwnd);


	int frameCount = 0;
	int fps = 0;
	double frameTime;
private:

	IDirectInputDevice8 *DiKeyboard;
	IDirectInputDevice8 *DiMouse;

	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;

	float rotx = 0;
	float rotz = 0;
	float scaleX = 1.0f;
	float scaleY = 1.0f;

	DirectX::XMMATRIX RotationX;
	DirectX::XMMATRIX RotationY;

	//global declarations
	IDXGISwapChain *swapchain;  //Pointer to the swap chain interface
	ID3D11Device *dev;			//Pointer to the direct3D device interface
	HRESULT hr;				//Pointer to the HRESULT
	ID3D11DeviceContext *devcon; //the pointer to the direct3D device context
	ID3D11RenderTargetView *backbuffer; //Pointer to the render target
	ID3D11VertexShader *pVS; //the vertex shader
	ID3D11PixelShader *pPS; //the pixel shader
	ID3D11Buffer *pVBuffer; // The Vertex Buffer
	ID3D11Buffer *pIBuffer; //Index Buffer
	ID3D11InputLayout *pLayout; //Layout for shaders
	ID3D11Buffer *cbPerObjectBuffer; //Buffer to store constant buffer variables
	HWND hWnd;

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
	//global struct
	//struct VERTEX
	//{
	//	FLOAT normx, normy, normz; //normal position
	//	FLOAT x, y, z; //position
	//	D3DXCOLOR Colour; // Colour
	//};
	//function prototypes



};
// BasicRender.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "BasicRender.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

//#pragma comment (lib, "d3dcompiler.lib")
//#pragma comment (lib, "d3d11.lib")

#define MAX_LOADSTRING 100

// screen resolution
#define SCREEN_WIDTH 2560
#define SCREEN_HEIGHT 1440

// namespaces
using namespace DirectX;

// data type to describe a vertex
struct SimpleVertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Colour;
};

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

IDXGISwapChain *pSwapchain;    // pointer to the swap chain interface
ID3D11Device *pDev;            // pointer to our Direct3D device interface
ID3D11DeviceContext *pDevCon;  // pointer to our Direct3D device context
ID3D11RenderTargetView *pRenderTargetView;  // pointer to back buffer
ID3D11Buffer *pVertexBuffer;   // pointer to vertex buffer
ID3D11InputLayout *pVertexLayout;  // vertex input layout
ID3D11VertexShader *pVS;       // pointer to vertex shader
ID3D11PixelShader *pPS;        // pointer to pixel shader

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void InitD3D(HWND hWnd);     // set up and initialize d3d
void CleanD3D(void);            // close d3d and release memory
void RenderFrame(void);         // renders a single frame
void PrepareVBuffer(void);   // create shape to render in vertex buffer
void InitPipeline(void);     // load and prepare shaders and input layout

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_BASICRENDER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	hInst = hInstance; // Store instance handle in our global variable

	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };  // window rectangle
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!hWnd)
	{
		MessageBox(NULL,
				   L"Call to CreateWindow failed :(",
				   L"BasicRender",
				   NULL);

		return 1;
	}

	ShowWindow(hWnd, nCmdShow);

	// set up and initialize d3d
	InitD3D(hWnd);

	// start in fullscreen mode by default
	pSwapchain->SetFullscreenState(TRUE, NULL);

	MSG msg;
	// Main message loop:
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message==WM_QUIT)
				break;
		}
		RenderFrame();
	}

	CleanD3D();

	return (int) msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BASICRENDER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// function to initialize and prepare Direct3D for use
void InitD3D(HWND hWnd)
{
	// set initial parameters for swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Width = SCREEN_WIDTH;
	sd.BufferDesc.Height = SCREEN_HEIGHT;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching

	// create device by calling D3D11CreateDeviceAndSwapChain
	D3D11CreateDeviceAndSwapChain(NULL,
								  D3D_DRIVER_TYPE_HARDWARE,
								  NULL,
								  NULL,
								  NULL,
								  NULL,
								  D3D11_SDK_VERSION,
								  &sd,
								  &pSwapchain,
								  &pDev,
								  NULL,
								  &pDevCon);

	// get a pointer to the back buffer
	ID3D11Texture2D *pBackBuffer;
	pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// create a render-target view
	pDev->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
	pBackBuffer->Release();

	// bind the view
	pDevCon->OMSetRenderTargets(1, &pRenderTargetView, NULL);

	// setup the viewport
	D3D11_VIEWPORT vp = { 0 };
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = SCREEN_WIDTH;
	vp.Height = SCREEN_HEIGHT;

	pDevCon->RSSetViewports(1, &vp);

	InitPipeline();
	PrepareVBuffer();
}

void CleanD3D(void)
{
	// switch to windowed mode
	pSwapchain->SetFullscreenState(FALSE, NULL);

	// close and release COM objects
	pVertexLayout->Release();
	pVS->Release();
	pPS->Release();
	pVertexBuffer->Release();
	pSwapchain->Release();
	pRenderTargetView->Release();
	pDev->Release();
	pDevCon->Release();
}

void PrepareVBuffer(void)
{
	SimpleVertex OurVertices[] =
	{
		{ XMFLOAT3{ 0.0f, 0.5f,0.0f }, XMFLOAT3{ 0.5f, 0.5f, 1.0f } },
		{ XMFLOAT3{ 0.45f, -0.5f, 0.0f }, XMFLOAT3{ 0.5f, 0.0f, 0.5f } },
		{ XMFLOAT3{ -0.45f, -0.5f, 0.0f }, XMFLOAT3{ 0.1f, 0.5f, 0.5f } }
	};

	// fill in a buffer description
	D3D11_BUFFER_DESC bufferDesc = { 0 };

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(SimpleVertex) * 3;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// create buffer
	pDev->CreateBuffer(&bufferDesc, NULL, &pVertexBuffer);

	// create mapped subresource to enable us to access the buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	// Disable GPU access to the vertex buffer data.
	pDevCon->Map(pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	// update vertex buffer
	memcpy(mappedSubresource.pData, OurVertices, sizeof(OurVertices));
	// Reenable GPU access to the vertex buffer data.
	pDevCon->Unmap(pVertexBuffer, 0);
}

void InitPipeline(void)
{
	// load shaders
	ID3DBlob *VS, *PS;
	D3DCompileFromFile(L"VertexShader.hlsl", 0, 0, "main", "vs_4_0", 0, 0, &VS, 0);
	D3DCompileFromFile(L"PixelShader.hlsl", 0, 0, "main", "ps_4_0", 0, 0, &PS, 0);

	// encapsulate both shaders in shader objects
	pDev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	pDev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	// set the shader objects
	pDevCon->VSSetShader(pVS, 0, 0);
	pDevCon->PSSetShader(pPS, 0, 0);

	// create input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA}
	};

	// fill input layout object
	pDev->CreateInputLayout(layout, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pVertexLayout);
	// set the input layout
	pDevCon->IASetInputLayout(pVertexLayout);
}

void RenderFrame(void)
{
	// clear back buffer to black
	{
		XMFLOAT4 clearColour = { 0.0f,0.0f,0.0f,0.0f };
		pDevCon->ClearRenderTargetView(pRenderTargetView, (FLOAT *)&clearColour);
	}

	// set vertex buffers
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pDevCon->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	// specify primitive type
	pDevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw the vertex buffer to the back buffer
	pDevCon->Draw(3, 0);

	// switch the back buffer and the front buffer
	pSwapchain->Present(0, 0);
}
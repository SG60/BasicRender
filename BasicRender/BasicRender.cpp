// BasicRender.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "BasicRender.h"

#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")

#define MAX_LOADSTRING 100

// screen resolution
#define SCREEN_WIDTH 2560
#define SCREEN_HEIGHT 1440

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

IDXGISwapChain *pSwapchain;    // pointer to the swap chain interface
ID3D11Device *pDev;            // pointer to our Direct3D device interface
ID3D11DeviceContext *pDevcon;  // pointer to our Direct3D device context
ID3D11RenderTargetView *pRenderTargetView;  // pointer to back buffer

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitD3D(HWND hWnd);     // set up and initialize d3d
void CleanD3D(void);            // close d3d and release memory

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	HRESULT hr;

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
	hr = InitD3D(hWnd);

	if (FAILED(hr))
	{
		MessageBox(NULL,
				   L"Call to InitD3D failed :(",
				   L"BasicRender",
				   NULL);

		return 1;
	}

	// Main message loop:
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE));
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message==WM_QUIT) break;
		}
	}

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
HRESULT InitD3D(HWND hWnd)
{
	HRESULT hr;

	// set initial parameters for swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = SCREEN_WIDTH;
	sd.BufferDesc.Height = SCREEN_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	// request a feature level
	D3D_FEATURE_LEVEL FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
	UINT numFeatureLevelsRequested = 1;
	D3D_FEATURE_LEVEL FeatureLevelsSupported;

	// create device by calling D3D11CreateDeviceAndSwapChain
	hr = D3D11CreateDeviceAndSwapChain(NULL,
									   D3D_DRIVER_TYPE_HARDWARE,
									   NULL,
									   0,
									   &FeatureLevelsRequested,
									   numFeatureLevelsRequested,
									   D3D11_SDK_VERSION,
									   &sd,
									   &pSwapchain,
									   &pDev,
									   &FeatureLevelsSupported,
									   &pDevcon);

	// get a pointer to the back buffer
	ID3D11Texture2D *pBackBuffer;
	hr = pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
							   (LPVOID*)&pBackBuffer);

	// create a render-target view
	pDev->CreateRenderTargetView(pBackBuffer, NULL,
								 &pRenderTargetView);

	// bind the view
	pDevcon->OMSetRenderTargets(1, &pRenderTargetView, NULL);

	// setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = SCREEN_WIDTH;
	vp.Height = SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pDevcon->RSSetViewports(1, &vp);

	return hr;
}

void CleanD3D(void)
{
	// close and release COM objects
	pSwapchain->Release();
	pDev->Release();
	pDevcon->Release();
	pRenderTargetView->Release();
}
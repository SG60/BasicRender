#pragma once

/////////////////////
// INCLUDES
//
#include <dxgi.h>
#include <d3d11.h>
#include <DirectXMath.h>

//////////////////////////////
// D3DClass
//
// Class to handle all direct3d system functions.

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd,
					bool fullscreen, float screenDepth, float screenNear);
	void Shutdown();

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix);
	void GetWorldMatrix(DirectX::XMMATRIX& worldMatrix);
	void GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix);

	void GetVideoCardInfo(char* cardName, int& memory);

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_orthoMatrix;
};


#pragma once

/////////////////////////////
// CLASS INCLUDES
#include "D3DClass.h"

/////////////////////////////
// GLOBAL GRAPHICS SETTINGS
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.f;
const float SCREEN_NEAR = 0.1f;

///////////////////////////////
// GraphicsClass
//
// Class to encapsulate graphics functionality

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3DClass* m_D3D;
};


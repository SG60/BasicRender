#include "stdafx.h"

#include "SystemClass.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance,
				   _In_opt_ HINSTANCE hPrevInstance,
				   _In_ LPSTR lpCmdLine,
				   _In_ int nShowCmd)
{
	SystemClass* System;
	bool result;

	// create the system object
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// initialize and run the system object
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// shutdown and release the system object
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}
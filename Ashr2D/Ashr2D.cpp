// Ashr2D.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Ashr2D.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

		// Use HeapSetInformation to specify that the process should
		// terminate if the heap manager detects an error in any heap used
		// by the process.
		// The return value is ignored, because we want to continue running in the
		// unlikely event that HeapSetInformation fails
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		CAshrMain tmain= CAshrMain();
		if (SUCCEEDED(tmain.Initialize()))
		{
			tmain.MsgProc();
		}
		
		CoUninitialize();
	}
	
	return 0;
}


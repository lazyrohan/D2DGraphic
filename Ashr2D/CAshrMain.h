/*
Dfine basic Driect2D initialize function and provide interface for graphic drawing
*/


#pragma once
#include <Windows.h>
#include <atlstr.h>
//Basic function header
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include <vector>
//Driect2D libs
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
//Image codec lib 
#include <wincodec.h>

#pragma comment(lib,"d2d1")
#pragma comment(lib,"dwrite")

//Safe release function. need?
template <class Interface> 
inline void SafeRelease(Interface **ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = NULL;
	}
		

}

//Debug output
#ifndef Assert
#if defined(DEBUG) || defined(_DEBUG)
#define Assert(b) do { if(!b){OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else 
#define Assert(b)
#endif
#endif

//?????
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

//Main body class
class CAshrMain
{
public:
	CAshrMain();
	virtual ~CAshrMain();

public:
	HRESULT Initialize(HWND hShowWnd=NULL);
	int MsgProc();
	//Resource created for each scene, open public interface for use
	HRESULT CreateDevDependRes(HWND hwd);

	//Create default window if not exist 
private:
	HRESULT CreateDefaultWnd( );
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wpm, LPARAM lpm);
	HRESULT OnDrawing(HWND hwd);
	HRESULT OnResize(LPARAM lpm);
	void ReleaseDevRes(void);
	

private:
	HWND mHwnd;
	ID2D1Factory* mpFactory;
	ID2D1HwndRenderTarget* mpRendertarget;
	std::vector < ID2D1SolidColorBrush*> mpBrushList;
};


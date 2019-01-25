#include "CAshrMain.h"


CAshrMain::CAshrMain() :
	mHwnd(NULL),
	mpFactory(NULL),
	mpRendertarget(NULL)
{

}


CAshrMain::~CAshrMain()
{
	mHwnd = NULL;
	SafeRelease(&mpFactory);
	SafeRelease(&mpRendertarget);
}

HRESULT CAshrMain::Initialize(HWND hShowWnd)
{
	HRESULT hr = S_OK;

	// Check show window
	if (hShowWnd == NULL)
	{
		//MessageBox(NULL, L"Not defined right window, will create default on.", L"Wrong Parameter", MB_YESNO);
		hr = CreateDefaultWnd();
		if (FAILED(hr))
			return hr;
	}
	else
		mHwnd = hShowWnd;

	/*
	Initialize D2D
	1. Create an ID2D1Factory1
	2. Create an ID2D1Device and an ID2D1DeviceContext
	*/

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mpFactory);
	//Create rander target
	if (SUCCEEDED(hr))
	{
		RECT rc;
		GetClientRect(mHwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
		hr = mpFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(mHwnd, size), &mpRendertarget);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Render target failed", L"GO", MB_YESNO);
			return hr;
		}

	}
	else
	{
		MessageBox(NULL, L"Factory failed", L"wrong", MB_YESNO);
	}

	return hr;
}

int CAshrMain::MsgProc()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

HRESULT CAshrMain::CreateDefaultWnd()
{
	HRESULT hr = S_OK;

	// register windows class
	WNDCLASSEXW wce = {};
	wce.cbSize = sizeof(WNDCLASSEX);
	wce.style = CS_HREDRAW | CS_VREDRAW;
	wce.lpfnWndProc = WndProc;
	wce.cbClsExtra = 0;
	wce.cbWndExtra = 0;
	wce.hInstance = NULL;
	wce.hbrBackground = NULL;
	wce.lpszMenuName = NULL;
	wce.hCursor = LoadCursor(NULL, IDI_APPLICATION);
	wce.lpszClassName = L"ShowWindow";

	RegisterClassEx(&wce);

	//Create window
	HWND hw = CreateWindowExW(
		0,
		L"ShowWindow",
		L"Show window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		HINST_THISCOMPONENT,
		NULL);
	DWORD er = GetLastError();
	CString str = "";
	str.Format(L"%d", er);
	//MessageBox(NULL, str, L"GO", MB_YESNO);
	//mHwnd = hw;
	//Show window
	//hr = mHwnd ? S_OK : E_FAIL;

	if (SUCCEEDED(hr))
	{

		ShowWindow(hw, SW_SHOWNORMAL);
		UpdateWindow(hw);
	}

	return hr;
}

LRESULT CAshrMain::WndProc(HWND hwnd, UINT msg, WPARAM wpm, LPARAM lpm)
{

	//MessageBox(NULL, L"Hi there msg", L"GO", MB_YESNO);
	if (msg == WM_CREATE)
	{
		//MessageBox(NULL, L"Hi there create", L"GO", MB_YESNO);
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lpm;
		CAshrMain *pDemoApp = (CAshrMain *)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			PtrToUlong(pDemoApp)
		);
		return 0;
	}
	else
	{
		//MessageBox(NULL, L"Hi there else", L"GO", MB_YESNO);
		CAshrMain *pDemoApp = reinterpret_cast<CAshrMain *>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA
			)));

		if (true)
		{
			switch (msg)
			{
				//MessageBox(NULL, L"Hi there switch", L"GO", MB_YESNO);
			case WM_SIZE:
			{
				//MessageBox(NULL, L"Hi there size", L"GO", MB_YESNO);
				pDemoApp->OnResize(lpm);
			}
			break;

			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hwnd, NULL, false);
			}
			break;

			case WM_PAINT:
			{
				//MessageBox(NULL, L"Hi there paint", L"GO", MB_YESNO);
				//pDemoApp->OnDrawing();
				ValidateRect(hwnd, NULL);
			}
			break;

			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			break;

			default:
				return ::DefWindowProc(hwnd, msg, wpm, lpm);

			}
		}
		//MessageBox(NULL, L"Hi there end", L"GO", MB_YESNO);
		return ::DefWindowProc(hwnd, msg, wpm, lpm);
	}

}

void CAshrMain::OnDrawing()
{
	HRESULT hr = S_OK;
	if (!mpRendertarget)
	{
		//MessageBox(NULL, L"Invalid render target", L"ParameterWrong", MB_YESNO);
		RECT rc;
		GetClientRect(mHwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
		hr = mpFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(mHwnd, size), &mpRendertarget);
	}

	
	//Create resource 
	ID2D1SolidColorBrush* pbrush = NULL;
	hr = mpRendertarget->CreateSolidColorBrush(D2D1::ColorF::ColorF(100.0f, 200.0f, 100.0f, 1.0f), &pbrush);
	mpRendertarget->BeginDraw();
	mpRendertarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 100.0f, 100.0f), pbrush);
	mpRendertarget->DrawRectangle(D2D1::RectF(0.0f, 0.0f, 100.0f, 100.0f), pbrush);
	mpRendertarget->EndDraw();
}

void CAshrMain::OnResize(LPARAM lpm)
{
	if (!mpRendertarget)
	{
		mpRendertarget->Resize(D2D1::SizeU((UINT)LPWORD(lpm), (UINT)HIWORD(lpm)));
	}
}


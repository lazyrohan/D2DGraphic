#include "CAshrMain.h"


CAshrMain::CAshrMain() :
	mHwnd(NULL),
	mpFactory(NULL),
	mpRendertarget(NULL)
{
	for (auto itr : mpBrushList)
		itr = NULL;
}


CAshrMain::~CAshrMain()
{
	ReleaseDevRes();
	SafeRelease(&mpFactory);
	mHwnd = NULL;

}

HRESULT CAshrMain::Initialize(HWND hShowWnd)
{
	HRESULT hr = S_OK;
	/*
Initialize D2D
1. Create an ID2D1Factory1
2. Create an ID2D1Device and an ID2D1DeviceContext
*/

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mpFactory);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create actory failed", L"Oohoo wrong", MB_YESNO);
		return hr;
	}

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

	//hr = CreateDevDependRes(mHwnd);

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

	//Get system screen size
	RECT sysrc;
	//Used for set system enviroment
	SystemParametersInfoW(SPI_GETWORKAREA, 0, &sysrc, 0);
	int startx = (int)((sysrc.right - sysrc.left) * 0.125);
	int starty = sysrc.top;
	//Create window
	mHwnd = CreateWindowExW(
		0,
		L"ShowWindow",
		L"Show window",
		WS_OVERLAPPEDWINDOW,
		startx, starty,
		(int)((sysrc.right - sysrc.left) * 0.75), sysrc.bottom - sysrc.top,
		NULL,
		NULL,
		HINST_THISCOMPONENT,
		this);
	/*DWORD er = GetLastError();
	CString str = "";
	str.Format(L"%d", er);*/
	//MessageBox(NULL, str, L"GO", MB_YESNO);
	//mHwnd = hw;
	//Show window
	hr = mHwnd ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		ShowWindow(mHwnd, SW_SHOWNORMAL);
		UpdateWindow(mHwnd);
	}

	return hr;
}

LRESULT CAshrMain::WndProc(HWND hwnd, UINT msg, WPARAM wpm, LPARAM lpm)
{
	CAshrMain* pthis = NULL;
	//MessageBox(NULL, L"Hi there msg", L"GO", MB_YESNO);
	if (msg == WM_CREATE)
	{
		//MessageBox(NULL, L"Hi there create", L"GO", MB_YESNO);
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lpm;
		pthis = (CAshrMain*)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			PtrToUlong(pthis)
		);
		return 0;
	}
	else
	{
		pthis = reinterpret_cast<CAshrMain*>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA
			)));
	}

	if (pthis)
	{
		switch (msg)
		{
			//MessageBox(NULL, L"Hi there switch", L"GO", MB_YESNO);
		case WM_SIZE:
		{
			//MessageBox(NULL, L"Hi there size", L"GO", MB_YESNO);
			pthis->OnResize(lpm);
			InvalidateRect(hwnd, NULL, false);
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
			pthis->OnDrawing(hwnd);
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
	return ::DefWindowProc(hwnd, msg, wpm, lpm);

}

HRESULT CAshrMain::OnDrawing(HWND hwd)
{
	HRESULT hr = S_OK;
	//ID2D1HwndRenderTarget* mpRendertarget = NULL;

	if (!mpRendertarget)
	{
		hr = CreateDevDependRes(mHwnd);
	}


	//Create resource
	if (SUCCEEDED(hr))
	{
		mpRendertarget->BeginDraw();
		mpRendertarget->Clear(D2D1::ColorF(0.0f, 0.05f, 0.0f, 1.0f)); //[0.0 1.0]
		mpRendertarget->FillRectangle(D2D1::RectF(210.0f, 210.0f, 400.0f, 400.0f), mpBrushList[0]);

		for (int i = 0; i < 40; i++)
		{
			mpRendertarget->DrawRectangle(D2D1::RectF(10.0f + i * 10, 10.0f + i * 10, 600.0f - i * 10, 600.0f - i * 10), mpBrushList[1],5.0f);
		}
		hr = mpRendertarget->EndDraw();

		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			ReleaseDevRes();
		}
	}

	return hr;
}

HRESULT CAshrMain::OnResize(LPARAM lpm)
{
	HRESULT hr = S_FALSE;
	if (mpRendertarget)
	{
		hr = mpRendertarget->Resize(D2D1::SizeU(LOWORD(lpm), HIWORD(lpm)));
	}

	return hr;
}

void CAshrMain::ReleaseDevRes(void)
{
	for (auto itr : mpBrushList)
		SafeRelease(&itr);
	mpBrushList.clear();
	SafeRelease(&mpRendertarget);
}

HRESULT CAshrMain::CreateDevDependRes(HWND hwd)
{
	if (!hwd)
	{
		MessageBoxW(NULL, L"Null HWND to create rendertarget resource ", L"ParameterWrong", NULL);
		return S_FALSE;
	}
	HRESULT hr = S_OK;

	if (!mpRendertarget)
	{
		//MessageBox(NULL, L"Invalid render target", L"ParameterWrong", MB_YESNO);
		RECT rc;
		GetClientRect(hwd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		hr = mpFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwd, size), &mpRendertarget);
	}

	if (SUCCEEDED(hr))
	{
		//Create brush
		ID2D1SolidColorBrush* pbrush = NULL;
		hr = mpRendertarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(0.0f, 1.0f, 1.0f, 1.0f)), &pbrush);
		mpBrushList.push_back(pbrush);
		/*ID2D1LinearGradientBrush* gbrush = NULL;
		ID2D1GradientStopCollection* pgstop = NULL;
		D2D1_GRADIENT_STOP gstop[3];
		gstop[0].color = D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f);
		gstop[0].position = 0.0f;
		gstop[1].color = D2D1::ColorF(0.0f, 1.0f, 0.0f, 1.0f);
		gstop[1].position = 0.5f;
		gstop[2].color = D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f);
		gstop[2].position = 1.0f;
		hr = mpRendertarget->CreateGradientStopCollection(gstop, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pgstop);
		if (SUCCEEDED(hr))
			hr = mpRendertarget->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0.0f, 0.0f), D2D1::Point2(100.0f, 100.0f)), pgstop, &gbrush);*/
		hr = mpRendertarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)), &pbrush);
		mpBrushList.push_back(pbrush);
		if (FAILED(hr))
			MessageBox(NULL, L"Create brush failed", L"ParameterWrong", MB_YESNO);
		ID2D1Image* pImg=NULL;
	}

	return hr;
}




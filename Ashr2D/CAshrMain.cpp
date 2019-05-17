#include "CAshrMain.h"


CAshrMain::CAshrMain() :
	mHwnd(NULL),
	mpFactory(NULL),
	mpWICFactory(NULL),
	mpWriteFactory(NULL),
	mpTextFmt(NULL),
	mpRendertarget(NULL),
	mpBitmap(NULL),
	mMousePos(D2D1::Point2F()),
	mMouseMoveRC(D2D1::RectF())
{
	for (auto itr : mpBrushList)
		itr = NULL;
}


CAshrMain::~CAshrMain()
{
	ReleaseDevRes();
	//SafeRelease(&mpBitmap);
	SafeRelease(&mpFactory);
	//SafeRelease(&mpWICFactory);
	mHwnd = NULL;

}

HRESULT CAshrMain::Initialize(HWND hShowWnd)
{
	HRESULT hr = S_OK;
	hr = CreateIndependRes();
	if (FAILED(hr))
		return hr;

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
	WNDCLASSEXW wce = {0};
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
		//SetWindowLongW(mHwnd, GWL_STYLE, WS_OVERLAPPED | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
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
		case WM_CREATE:
		{
		}
		break;

		case WM_SIZE:
		{
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

		case WM_MOUSEMOVE:
		{
			pthis->OnMouseMove(lpm, wpm);
		}
		break;

		case WM_LBUTTONDOWN:
		{
			pthis->OnLbtnDown(lpm, wpm);
		}
		break;

		case WM_LBUTTONUP:
		{
			pthis->OnLbtnUp();
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
		//mpRendertarget->FillRectangle(D2D1::RectF(210.0f, 210.0f, 400.0f, 400.0f), mpBrushList[0]);

		//for (int i = 0; i < 40; i++)
		//{
		//	mpRendertarget->DrawRectangle(D2D1::RectF(10.0f + i * 10, 10.0f + i * 10, 600.0f - i * 10, 600.0f - i * 10), mpBrushList[1], 5.0f);
		//}

		//

		//// Draw pics
		//// Load pics
		//hr = LoadImgFromFile(L"D:\\prog\\steam\\steamapps\\common\\Factorio\\data\\base\\graphics\\entity\\electric-furnace\\hr-electric-furnace.png");
		//if (SUCCEEDED(hr))
		//{
		//	float scaler = 1.0f;
		//	D2D1_SIZE_F imgSize = mpBitmap->GetSize();
		//	D2D1_RECT_F srcrect = D2D1::RectF(0, 0, imgSize.width, imgSize.height);
		//	D2D1_SIZE_F csize = mpRendertarget->GetSize();
		//	for (int j = 0; j < csize.height / (imgSize.height * scaler); j++)
		//	{
		//		for (int i = 0; i < csize.width / (imgSize.width * scaler); i++)
		//		{
		//			srcrect = D2D1::RectF(i * imgSize.width * scaler, j * imgSize.height * scaler, (i + 1) * imgSize.width * scaler, (1 + j) * imgSize.height * scaler);
		//			mpRendertarget->DrawBitmap(mpBitmap, srcrect);
		//			mpRendertarget->DrawRectangle(srcrect, mpBrushList[1], 0.5f);
		//		}
		//	}
		//}

		DrawRegPolygon(12, 100.0f, D2D1::Point2F(300.0f, 300.0f));

		if (mMouseMoveRC.right * mMouseMoveRC.bottom != 0.0f)
		{
			//MessageBox(NULL, L"L BTN come", L"GO", MB_YESNO);
			mpRendertarget->DrawRectangle(mMouseMoveRC, mpBrushList[1], 1.0f);
		}

		const wchar_t* wstr = L"Text rendered by DirectWrite和中文输出.";
		UINT32 strlen = wcslen(wstr);
		mpRendertarget->DrawTextW(wstr, strlen, mpTextFmt, mMouseMoveRC, mpBrushList[0], D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT, DWRITE_MEASURING_MODE_NATURAL);

			
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
	HRESULT hr = S_OK;
	if (mpRendertarget)
	{
		hr = mpRendertarget->Resize(D2D1::SizeU(LOWORD(lpm), HIWORD(lpm)));
	}

	return hr;
}

HRESULT CAshrMain::OnMouseMove(LPARAM lpm, WPARAM wpm)
{
	HRESULT hr = S_OK;

	// Get mouse position


	if (wpm&MK_LBUTTON)
	{
		D2D1_POINT_2F curPos = CaptureMousePos(lpm);
		mMouseMoveRC = D2D1::RectF(mMousePos.x, mMousePos.y,curPos.x, curPos.y );
		InvalidateRect(mHwnd, NULL, FALSE);
	}

	return hr;
}

HRESULT CAshrMain::OnLbtnDown(LPARAM lpm, WPARAM wpm)
{
	HRESULT hr = S_OK;
	SetCapture(mHwnd);
	mMousePos = CaptureMousePos(lpm);
	InvalidateRect(mHwnd, NULL, FALSE);

	return hr;
}

void CAshrMain::OnLbtnUp()
{
	ReleaseCapture();
}

void CAshrMain::ReleaseDevRes(void)
{
	for (auto itr : mpBrushList)
		SafeRelease(&itr);
	mpBrushList.clear();
	//SafeRelease(&mpBitmap);
	SafeRelease(&mpRendertarget);
}

void CAshrMain::ReleaseIndependRes(void)
{
	SafeRelease(&mpFactory);
	SafeRelease(&mpWriteFactory);
	SafeRelease(&mpTextFmt);
}

D2D1_POINT_2F CAshrMain::CaptureMousePos(LPARAM lpm)
{
	D2D1_POINT_2F mpos = D2D1::Point2F();
	if (mpFactory)
	{
		FLOAT dpix = 0.0f;
		FLOAT dpiy = 0.0f;
		mpFactory->GetDesktopDpi(&dpix, &dpiy);
		mpos = D2D1::Point2F(((float)(short)LOWORD(lpm)) * 96.0f / dpix, ((float)(short)HIWORD(lpm)) * 96.0f / dpiy);

	}
	return mpos;
}

void CAshrMain::DrawRegPolygon(int sides, float radius, D2D1_POINT_2F centerPos)
{
	//Check parameters
	if (sides < 2 || radius <= 0)
		return;
	if (radius>centerPos.x)
	{
		centerPos.x = radius;
	}
	if (radius>centerPos.y)
	{
		centerPos.y = radius;
	}

// Fill points series
	std::vector<D2D1_POINT_2F> polyPoints;
	//polyPoints.resize(sides);
	for (int i = 0; i <sides; i++)
	{
		double angle = (2.0 *i / sides + 1.0 / sides)*3.14159;
		polyPoints.push_back(D2D1::Point2F(centerPos.x-radius*sin(angle),centerPos.y+radius*cos(angle)));
		
	}

	const wchar_t* wstr = L"C";
	UINT32 strlen = wcslen(wstr);
	D2D1_RECT_F rc = D2D1::RectF(centerPos.x, centerPos.y, 10, 10);
	mpRendertarget->DrawTextW(wstr, strlen, mpTextFmt, rc, mpBrushList[0], D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT, DWRITE_MEASURING_MODE_NATURAL);
	// Draw lines
	wstr = L"P";
	strlen = wcslen(wstr);
	if (mpRendertarget)
	{
		
		for (int i=0;i<sides;i++)
		{
			mpRendertarget->DrawLine(centerPos, polyPoints[i], mpBrushList[0]);
			mpRendertarget->DrawLine(polyPoints[i], polyPoints[(i+1==sides?0:i+1)], mpBrushList[0]);
			rc= D2D1::RectF(polyPoints[i].x, polyPoints[i].y, 10, 10);
			mpRendertarget->DrawTextW(wstr, strlen, mpTextFmt, rc, mpBrushList[0], D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT, DWRITE_MEASURING_MODE_NATURAL);
		}

		
		
	}
	polyPoints.clear();
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
		ID2D1Image* pImg = NULL;
	}

	return hr;
}

HRESULT CAshrMain::CreateIndependRes()
{
	HRESULT hr = S_OK;
	/*
Initialize D2D
1. Create an ID2D1Factory1
2. Create an ID2D1Device and an ID2D1DeviceContext
*/
	//create render factory
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mpFactory);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create factory failed", L"Oohoo wrong", MB_OK);
		return hr;
	}

	//create Image factory
	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)& mpWICFactory);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create WICImagingfactory failed", L"Oohoo wrong", MB_OK);
		return hr;
	}

	//create write factory
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**> (&mpWriteFactory));
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create IDwriteFactory failed", L"Oohoo wrong", MB_OK);
		return hr;
	}

	hr = mpWriteFactory->CreateTextFormat(L"Calibri",NULL,DWRITE_FONT_WEIGHT_REGULAR,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,18.0f,L"en-us",&mpTextFmt);
	if (SUCCEEDED(hr))
	{
		hr = mpTextFmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr))
	{
		hr = mpTextFmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	return hr;
}

HRESULT CAshrMain::LoadImgFromFile(LPCWSTR filename)
{
	HRESULT hr = S_OK;

	if (!mpWICFactory)
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)& mpWICFactory);
	}

	//1. Create WIC decoder  
	IWICBitmapDecoder* pDecoder = NULL;
	if (SUCCEEDED(hr))
	{
		hr = mpWICFactory->CreateDecoderFromFilename(filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);
	}

	//2. Retrieve a frame
	IWICBitmapFrameDecode* pFrmDecode = NULL;
	if (SUCCEEDED(hr))
	{
		hr = pDecoder->GetFrame(0, &pFrmDecode);
	}
	//3. Covert to D2D format
	IWICFormatConverter* pConverter = NULL;
	if (SUCCEEDED(hr))
	{
		hr = mpWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(pFrmDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
	}

	if (SUCCEEDED(hr))
	{
		mpRendertarget->CreateBitmapFromWicBitmap(pConverter, &mpBitmap);
	}
	SafeRelease(&pConverter);
	SafeRelease(&pFrmDecode);
	SafeRelease(&pDecoder);
	return hr;
}




// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//+-----------------------------------------------------------------------------
//
//
//  $Description:
//      Sample DirectWrite Application
//
//  $ENDTAG
//
//------------------------------------------------------------------------------

#include "d2dui.h"
#include "Main.h"
#include "Array.h"
#include "MathUtil.h"
#include "TimeUtil.h"
#include "string_cvt.h"
#include "time_cvt.h"
#include "../src/timer.h"
/******************************************************************
*                                                                 *
* Provides the entry point for the application.                   *
*                                                                 *
******************************************************************/

int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR  lpCmdLine ,
    int /* nCmdShow */
    )
{
    {
        prudens::timer t;
        uint32_t count = 1;
        for ( int i = 0; i < 10; i++ )
        {
            t.start( i * 100, true, [&] ( uint32_t time_id, void*userdata )
            {
                _CrtDbgReport( _CRT_WARN, __FILE__, __LINE__, "min_max_heap", "timer is coming %d\n", count++ );

                t.remove( time_id );
            }, nullptr );
        }
        Sleep( 5000 );
    }

    // Ignore the return value because we want to continue running even in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            DemoApp app;

            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}

/******************************************************************
*                                                                 *
*  Initialize members.                                            *
*                                                                 *
******************************************************************/

DemoApp::DemoApp() :
    m_hwnd(NULL),
    m_pD2DFactory(NULL),
    m_pWICFactory(NULL),
    m_pBitmap(NULL),
    m_pRenderTarget(NULL),
    m_pBlackBrush(NULL),
    m_pYellowGreenBrush(NULL),
    m_pLinearGradientBrush(NULL),
    m_pRadialGradientBrush(NULL),
    m_pBitmapBrush(NULL),
    m_pDWriteFactory(NULL),
    m_pTextFormat(NULL)
{
}

/******************************************************************
*                                                                 *
*  Release resources.                                             *
*                                                                 *
******************************************************************/

DemoApp::~DemoApp()
{
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pWICFactory);
    SafeRelease(&m_pBitmap);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pBlackBrush);
    SafeRelease(&m_pYellowGreenBrush);
    SafeRelease(&m_pLinearGradientBrush);
    SafeRelease(&m_pRadialGradientBrush);
    SafeRelease(&m_pBitmapBrush);
    SafeRelease(&m_pDWriteFactory);
    SafeRelease(&m_pTextFormat);
}

/******************************************************************
*                                                                 *
*  Create theapplication window and device-independent resources. *
*                                                                 *
******************************************************************/

HRESULT DemoApp::Initialize()
{
    {
         wavebuf_ = waveform_main( __argc, __argv );
    }
    HRESULT hr;

    hr = CreateDeviceIndependentResources();
    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = DemoApp::WndProc;
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = sizeof(LONG_PTR);
        wcex.hInstance     = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName  = NULL;
        wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wcex.lpszClassName = L"D2DDemoApp";

        RegisterClassEx(&wcex);

        // Create the application window.
        //
        // Because the CreateWindow function takes its size in pixels, we
        // obtain the system DPI and use it to scale the window size.
        FLOAT dpiX, dpiY;
        m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        m_hwnd = CreateWindow(
            L"D2DDemoApp",
            L"Direct2D Demo App",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this
            );

        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            ShowWindow(m_hwnd, SW_SHOWNORMAL);

            UpdateWindow(m_hwnd);
            ::SetTimer( m_hwnd, 1000, 25, [] ( HWND hwnd, UINT id, UINT_PTR event, DWORD v )
            {
                SendMessage( hwnd, WM_PAINT, 0, 0 );
            } );
        }
    }

    return hr;
}

void DemoApp::OnTimer( HWND hwnd, UINT,UINT_PTR, DWORD )
{

}
/******************************************************************
*                                                                 *
*  This method is used to create resources which are not bound    *
*  to any device. Their lifetime effectively extends for the      *
*  duration of the app.                                           *
*                                                                 *
******************************************************************/
HRESULT DemoApp::CreateDeviceIndependentResources()
{
    HRESULT hr;

    // Create a WIC factory.
    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_pWICFactory)
        );

    if (SUCCEEDED(hr))
    {
        // Create a Direct2D factory.
        hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            &m_pD2DFactory
            );
    }

    if (SUCCEEDED(hr))
    {
        // Create a shared DirectWrite factory.
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
            );

    }

    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite text format object.
        hr = m_pDWriteFactory->CreateTextFormat(
            L"Verdana", //font family name
            NULL, //font collection (NULL sets it to use the system font collection)
            DWRITE_FONT_WEIGHT_BOLD,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            10.5f,
            L"en-us",
            &m_pTextFormat
            );
       if(SUCCEEDED(hr)) m_pTextFormat->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_CENTER );
    }
		
    return hr;
}

/******************************************************************
*                                                                 *
*  DemoApp::CreateDeviceResources                                 *
*                                                                 *
*  This method creates resources which are bound to a particular  *
*  D3D device. It's all centralized here, in case the resources   *
*  need to be recreated in case of D3D device loss (eg. display   *
*  change, remoting, removal of video card, etc).                 *
*                                                                 *
******************************************************************/
HRESULT DemoApp::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
            );

        // Create a Direct2D render target.
        hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties( D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat( DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED ) ),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
            );

        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
                &m_pBlackBrush
                );
        }

        // Create a solid color brush with its rgb value 0x9ACD32.
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF(0x9ACD32, 0.4f)),  
                &m_pYellowGreenBrush
                );
        }


        if (SUCCEEDED(hr))
        {
            // Create an array of gradient stops to put in the gradient stop
            // collection that will be used in the gradient brush.
            ID2D1GradientStopCollection *pGradientStops = NULL;

            D2D1_GRADIENT_STOP gradientStops[3];
            gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Yellow, 1);
            gradientStops[0].position = 0.0f;
            gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::ForestGreen, 1);
            gradientStops[1].position = 0.5f;
			gradientStops[ 2 ].color = D2D1::ColorF( D2D1::ColorF::BurlyWood, 1 );
			gradientStops[ 2 ].position = 1.0f;
            // Create the ID2D1GradientStopCollection from a previously
            // declared array of D2D1_GRADIENT_STOP structs.
            hr = m_pRenderTarget->CreateGradientStopCollection(
                gradientStops,
                3,
                D2D1_GAMMA_2_2,
                D2D1_EXTEND_MODE_CLAMP,
                &pGradientStops
                );

            // The line that determines the direction of the gradient starts at
            // the upper-left corner of the square and ends at the lower-right corner.

            if (SUCCEEDED(hr))
            {
                hr = m_pRenderTarget->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties( //�������ʼ���յ㡣
                        D2D1::Point2F(0, 0),
                        D2D1::Point2F(150, 150)),
                    pGradientStops,
                    &m_pLinearGradientBrush
                    );
            }

            // The center of the gradient is in the center of the box.
            // The gradient origin offset was set to zero(0, 0) or center in this case.
            if (SUCCEEDED(hr))
            {
                hr = m_pRenderTarget->CreateRadialGradientBrush(
                    D2D1::RadialGradientBrushProperties(
                        D2D1::Point2F(75, 75),
                        D2D1::Point2F(30, -30),// ����ƫ����ʼ��
                        75,
                        75),
                    pGradientStops,
                    &m_pRadialGradientBrush
                    );
            }

            SafeRelease(&pGradientStops);
        }

        // Create the bitmap to be used by the bitmap brush.
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"FERN",
                L"Image",
                &m_pBitmap
                );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateBitmapBrush(
                m_pBitmap,
                &m_pBitmapBrush
                );
        }
    }
    return hr;
}

/******************************************************************
*                                                                 *
*  Discard device-specific resources which need to be recreated   *
*  when a Direct3D device is lost.                                *
*                                                                 *
******************************************************************/

void DemoApp::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pBlackBrush);
    SafeRelease(&m_pYellowGreenBrush);
    SafeRelease(&m_pLinearGradientBrush);
    SafeRelease(&m_pRadialGradientBrush);
    SafeRelease(&m_pBitmapBrush);

}

/******************************************************************
*                                                                 *
*  The main window message loop.                                  *
*                                                                 *
******************************************************************/

void DemoApp::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}



/******************************************************************
*                                                                 *
*  Called whenever the application needs to display the client    *
*  window.                                                        *
*                                                                 *
*  Note that this function will automatically discard             *
*  device-specific resources if the Direct3D device disappears    *
*  during execution, and will recreate the resources the          *
*  next time it's invoked.                                        *
*                                                                 *
******************************************************************/
HRESULT DemoApp::OnRender()
{

    HRESULT hr = CreateDeviceResources();
    D2D1_POINT_2F start = D2D1::Point2F( 100.0f, 200.f );
    int width = 1000;
    int height = 250;
   
    if (SUCCEEDED(hr))
    {
        static const WCHAR szSolidBrushText[] = L"ID2D1SolidColorBrush";
        static const WCHAR szBitmapBrushText[] = L"ID2D1BitmapBrush";
        static const WCHAR szLinearGradientText[] = L"ID2D1LinearGradientBrush";
        static const WCHAR szRadialGradientText[] = L"ID2D1RadialGradientBrush";

        m_pRenderTarget->BeginDraw();
 		MARGINS ins = { 0,0,200,200 };
 		DwmExtendFrameIntoClientArea(m_hwnd,&ins);
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

        // Define the shape of rectangles to be filled with brushes. 
        D2D1_RECT_F rcBrushRect = D2D1::RectF(5, 0, 150, 150);
        //D2D1_RECT_F rcBrushRect = D2D1::RectF( 5, 0, 5, 150 );
        // Define the area where captions are drawn.
        D2D1_RECT_F rcTextRect = D2D1::RectF(5, 165, 175, 200);
   
        // Start with a white background.
		m_pRenderTarget->Clear( D2D1::ColorF( D2D1::ColorF::White, 1.0f ) );
// 
        // Translate for the solid color brush.
//         m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(5, 5)));
// 
//         // Demonstrate a solid color brush.
//         m_pRenderTarget->FillRectangle(&rcBrushRect, m_pYellowGreenBrush);
//         m_pRenderTarget->DrawRectangle(&rcBrushRect, m_pBlackBrush, 1, NULL);
// 		D2D1_RECT_F rcClip = D2D1::RectF( 5, 0, 100, 200 );
// 		m_pRenderTarget->PushAxisAlignedClip( rcClip, D2D1_ANTIALIAS_MODE_ALIASED );
//         // Draw a caption.
//         m_pRenderTarget->DrawText(
//             szSolidBrushText,
//             ARRAYSIZE(szSolidBrushText) - 1,
//             m_pTextFormat,
//             &rcTextRect,
//             m_pBlackBrush
//             );
// 		m_pRenderTarget->PopAxisAlignedClip();
//         // Translate for the linear gradient brush.
//         m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(200, 5)));
// 		
// 		m_pRenderTarget->SetTransform(
// 			D2D1::Matrix3x2F::Rotation(
// 			45.0f,
// 			D2D1::Point2F( 160.0, 320.0f ) )
// 			);
//         // Demonstrate a linear gradient brush.
//         m_pRenderTarget->FillRectangle(&rcBrushRect, m_pLinearGradientBrush);
//         m_pRenderTarget->DrawRectangle(&rcBrushRect, m_pBlackBrush, 1, NULL);
// 		m_pRenderTarget->SetTransform(
// 			D2D1::Matrix3x2F::Rotation(
// 			45.0f,
// 			D2D1::Point2F( 290.0, 180.0f ) )
// 			);
//         // Draw a caption.
//         m_pRenderTarget->DrawText(
//             szLinearGradientText,
//             ARRAYSIZE(szLinearGradientText) - 1,
//             m_pTextFormat,
//             &rcTextRect,
//             m_pBlackBrush
//             );
//    
//         m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
// 
//         // Translate for the radial gradient brush.
//         m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(5, 200)));
// 
//         // Draw a caption.
//         m_pRenderTarget->DrawText(
//             szRadialGradientText,
//             ARRAYSIZE(szRadialGradientText) - 1,
//             m_pTextFormat,
//             &rcTextRect,
//             m_pBlackBrush
//             );
// 
//         D2D1_ELLIPSE   ellipse = D2D1::Ellipse(D2D1::Point2F(75.f, 75.f), 75.f, 75.f);
// 
//         m_pRenderTarget->FillEllipse(ellipse, m_pRadialGradientBrush);
//         m_pRenderTarget->DrawEllipse(ellipse, m_pBlackBrush, 1, NULL);
// 
//         // Translate for the bitmap brush.
//         m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(200, 200)));
// 
//         // Draw a caption.
//         m_pRenderTarget->DrawText(
//             szBitmapBrushText,
//             ARRAYSIZE(szBitmapBrushText) - 1,
//             m_pTextFormat,
//             &rcTextRect,
//             m_pBlackBrush
//             );
//         
//         // Demonstrate a bitmap brush.
// 		//m_pBitmapBrush->SetTransform( D2D1::Matrix3x2F::Translation( 50.0, 0.0 ));
//         //m_pRenderTarget->FillRectangle(&rcBrushRect, m_pBitmapBrush);
//         m_pRenderTarget->DrawRectangle(&rcBrushRect, m_pBlackBrush, 1, NULL);
        m_pRenderTarget->DrawRectangle( D2D1::RectF( start.x, start.y, start.x+width, start.y+height ),m_pBlackBrush );
       int size = wavebuf_.getSize();
       FLOAT center = start.y + height;
       for ( int i = 0; i < size && i < width; i++ )
       {
           int low = wavebuf_.getMinSample( i ) + 32768;
           int high = wavebuf_.getMaxSample( i ) + 32768;

           // scale to fit the bitmap
           int low_y = center - low * height / 65536;
           int high_y = center - high * height / 65536;
           m_pRenderTarget->DrawLine( D2D1::Point2F( start.x+i+0.5, low_y ), D2D1::Point2F( start.x+i+0.5, high_y ), m_pBlackBrush, 0.5f );
       }

       drawTimeAxisLabels();
        hr = m_pRenderTarget->EndDraw();

        if (hr == D2DERR_RECREATE_TARGET)
        {
            hr = S_OK;
            DiscardDeviceResources();
        }
    }

    return hr;
}

void DemoApp::drawTimeAxisLabels() const
{
    D2D1_POINT_2F start = D2D1::Point2F( 100.0f, 200.f );
    int width = 1000;
    int height = 250;

    const int marker_height = 10;
    int start_time_ = 0;
    // Time interval between axis markers (seconds)
    const int axis_label_interval_secs = getAxisLabelScale();

    // Distance between axis markers (pixels)
    const int axis_label_interval_pixels = secondsToPixels( axis_label_interval_secs );

    // Time of first axis marker (seconds)
    const int first_axis_label_secs = MathUtil::roundUpToNearest( start_time_, axis_label_interval_secs );

    // Distance between waveform start time and first axis marker (seconds)
    const double axis_label_offset_secs = first_axis_label_secs - start_time_;

    // Distance between waveform start time and first axis marker (samples)
    const int axis_label_offset_samples = secondsToSamples( axis_label_offset_secs );

    // Distance between waveform start time and first axis marker (pixels)
    const int axis_label_offset_pixels = axis_label_offset_samples / wavebuf_.getSamplesPerPixel();

    int secs = first_axis_label_secs;

    for ( ;; )
    {
        const int x = axis_label_offset_pixels +
            ( secs - first_axis_label_secs ) * wavebuf_.getSampleRate() / wavebuf_.getSamplesPerPixel();

        if (x == 0)
        {
            secs += axis_label_interval_secs;
            continue;
        }
        if ( x >= width )
        {
            break;
        }
        m_pRenderTarget->DrawLine( D2D1::Point2F(start.x+ x, start.y+height  ), D2D1::Point2F( start.x + x, start.y+height-10 ), m_pBlackBrush, 0.5f );
        m_pRenderTarget->DrawLine( D2D1::Point2F( start.x + x, start.y ), D2D1::Point2F( start.x + x, start.y  + 10 ), m_pBlackBrush, 0.5f );

        WCHAR wlabel[50] = {0};
        char label[50];
        const int label_length = TimeUtil::secondsToString( label, ARRAY_LENGTH( label ), secs );
        size_t len = strlen( label ) + 1;
        size_t converted = 0;
        mbstowcs_s( &converted, wlabel, len, label, _TRUNCATE );

        D2D1_RECT_F rcTextRect = D2D1::RectF( start.x + x-50, start.y + height - 30, start.x + x + 50, start.y + height);
        m_pRenderTarget->DrawText(
            wlabel,
            ARRAYSIZE( wlabel ) - 1,
            m_pTextFormat,
            &rcTextRect,
            m_pBlackBrush
            );



        secs += axis_label_interval_secs;
    }
}
/******************************************************************
*                                                                 *
*  If the application receives a WM_SIZE message, this method     *
*  resizes the render target appropriately.                       *
*                                                                 *
******************************************************************/

void DemoApp::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        // Note: This method can fail, but it's okay to ignore the
        // error here -- the error will be repeated on the next call to
        // EndDraw.
        m_pRenderTarget->Resize(D2D1::SizeU(width, height));
    }
}


/******************************************************************
*                                                                 *
*                                                                 *
*  The window message handler.                                    *
*                                                                 *
******************************************************************/

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(pDemoApp)
            );

        result = 1;
    }
    else
    {
        DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
                )));

        bool wasHandled = false;

        if (pDemoApp)
        {
            switch (message)
            {
            case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
                    pDemoApp->OnResize(width, height);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DISPLAYCHANGE:
                {
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_PAINT:
                {
                    TRACE( "wm_paint" );
                    pDemoApp->OnRender();
                    ValidateRect(hwnd, NULL);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DESTROY:
                {
                    PostQuitMessage(0);
                }
                result = 1;
                wasHandled = true;
                break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}


/******************************************************************
*                                                                 *
*                                                                 *
*  This method will create a Direct2D bitmap from a resource in   *
*  this DLL.                                                      *
*                                                                 *
******************************************************************/

HRESULT DemoApp::LoadResourceBitmap(
    ID2D1RenderTarget *pRenderTarget,
    IWICImagingFactory *pIWICFactory,
    PCWSTR resourceName,
    PCWSTR resourceType,
    ID2D1Bitmap **ppBitmap
    )
{
    HRESULT hr = S_OK;
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;

    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void *pImageFile = NULL;
    DWORD imageFileSize = 0;

    // Locate the resource.
    imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);

    hr = imageResHandle ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        // Load the resource.
        imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageResDataHandle ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Lock it to get a system memory pointer.
        pImageFile = LockResource(imageResDataHandle);

        hr = pImageFile ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Calculate the size.
        imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageFileSize ? S_OK : E_FAIL;

    }
    if (SUCCEEDED(hr))
    {
        // Create a WIC stream to map onto the memory.
        hr = pIWICFactory->CreateStream(&pStream);
    }
    if (SUCCEEDED(hr))
    {
        // Initialize the stream with the memory pointer and size.
        hr = pStream->InitializeFromMemory(
            reinterpret_cast<BYTE*>(pImageFile),
            imageFileSize
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a decoder for the stream.
        hr = pIWICFactory->CreateDecoderFromStream(
            pStream,
            NULL,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        hr = pConverter->Initialize(
            pSource,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0.f,
            WICBitmapPaletteTypeMedianCut
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
            );
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);

    return hr;
}

int DemoApp::getAxisLabelScale() const
{
    int base_secs = 1; // seconds

    const int steps[] = { 1, 2, 5, 10, 20, 30 };

    const int MIN_SPACING = 60; // pixels

    int index = 0;

    int secs;

    for ( ;; )
    {
        secs = base_secs * steps[index];

        int pixels = secondsToPixels( secs );

        if ( pixels < MIN_SPACING )
        {
            if ( ++index == ARRAY_LENGTH( steps ) )
            {
                base_secs *= 60; // seconds -> minutes -> hours
                index = 0;
            }
        }
        else
        {
            // Spacing OK
            break;
        }
    }

    return secs;
}

int DemoApp::secondsToPixels( const double seconds ) const
{
    return static_cast<int>( seconds * wavebuf_.getSampleRate() / wavebuf_.getSamplesPerPixel() );
}
#include "DX.h"

namespace D2D
{

IDWriteFactory *dwrite_factory = null;
ID2D1Factory2 *d2d_factory = null;
ID2D1Device1 *d2d_device = null;
ID2D1DeviceContext1 *device_context = null;
ID2D1Bitmap1 *d2d_bitmap = null;
IDXGISurface *dxgi_buffer = null;
ID2D1SolidColorBrush *black_brush = null;
ID2D1SolidColorBrush *white_brush = null;
ID2D1SolidColorBrush *red_brush = null;
IDWriteTextFormat *text_format = null;

HRESULT Open(bool debug, IDXGIDevice *dxgi_device, IDXGISwapChain *swap_chain)
{
    D2D1_FACTORY_OPTIONS options;

    if(debug) {
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    } else {
        options.debugLevel = D2D1_DEBUG_LEVEL_NONE;
    }

    D2D1_BITMAP_PROPERTIES1 p;
    p.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    p.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    p.dpiX = 96;
    p.dpiY = 96;
    p.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    p.colorContext = null;

    DXR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown **)&dwrite_factory));
    DXR(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory), &options, (void **)&d2d_factory));
    DXR(d2d_factory->CreateDevice(dxgi_device, &d2d_device));
    DXR(d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &device_context));
    DXR(swap_chain->GetBuffer(0, __uuidof(IDXGISurface), (void **)&dxgi_buffer));
    DXR(device_context->CreateBitmapFromDxgiSurface(dxgi_buffer, &p, &d2d_bitmap));
    device_context->SetTarget(d2d_bitmap);

/*
    DXR(device_context->CreateSolidColorBrush(D2D1::ColorF(params.blank_color), &black_brush));
    DXR(device_context->CreateSolidColorBrush(D2D1::ColorF(params.flash_color), &white_brush));
    DXR(device_context->CreateSolidColorBrush(D2D1::ColorF(0xff0000), &red_brush));
    DXR(dwrite_factory->CreateTextFormat(L"Consolas", null, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 18.0f, L"en-GB", &text_format));
    DXR(text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
    DXR(text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
*/
    return NOERROR;
}

//////////////////////////////////////////////////////////////////////

HRESULT Close()
{
    //Release(text_format);
    //Release(red_brush);
    //Release(white_brush);
    //Release(black_brush);
    Release(d2d_bitmap);
    Release(dxgi_buffer);
    Release(device_context);
    Release(d2d_device);
    Release(d2d_factory);
    Release(dwrite_factory);
    return NOERROR;
}

}    // namespace D2D

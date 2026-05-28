#include "dx11_window.h"

#include "imgui.h"
#include "imgui_impl_win32.h"

static Dx11Window* g_Window = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
);

bool CreateDx11Window(
    Dx11Window& window,
    const wchar_t* title,
    int width,
    int height,
    float dpiScale
)
{
    g_Window = &window;

    window.wc = {
        sizeof(window.wc),
        CS_CLASSDC,
        WndProc,
        0L,
        0L,
        GetModuleHandle(nullptr),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        title,
        nullptr
    };

    ::RegisterClassExW(&window.wc);

    window.hwnd = ::CreateWindowW(
        window.wc.lpszClassName,
        title,
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        static_cast<int>(width * dpiScale),
        static_cast<int>(height * dpiScale),
        nullptr,
        nullptr,
        window.wc.hInstance,
        nullptr
    );

    if (!window.hwnd)
        return false;

    if (!CreateDeviceD3D(window))
    {
        CleanupDeviceD3D(window);
        ::UnregisterClassW(window.wc.lpszClassName, window.wc.hInstance);
        return false;
    }

    return true;
}

void DestroyDx11Window(Dx11Window& window)
{
    CleanupDeviceD3D(window);

    if (window.hwnd)
    {
        ::DestroyWindow(window.hwnd);
        window.hwnd = nullptr;
    }

    if (window.wc.lpszClassName)
    {
        ::UnregisterClassW(window.wc.lpszClassName, window.wc.hInstance);
        window.wc = {};
    }

    if (g_Window == &window)
        g_Window = nullptr;
}

bool CreateDeviceD3D(Dx11Window& window)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));

    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = window.hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };

    HRESULT res = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevelArray,
        2,
        D3D11_SDK_VERSION,
        &sd,
        &window.swapChain,
        &window.device,
        &featureLevel,
        &window.deviceContext
    );

    if (res == DXGI_ERROR_UNSUPPORTED)
    {
        res = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            createDeviceFlags,
            featureLevelArray,
            2,
            D3D11_SDK_VERSION,
            &sd,
            &window.swapChain,
            &window.device,
            &featureLevel,
            &window.deviceContext
        );
    }

    if (res != S_OK)
        return false;

    CreateRenderTarget(window);
    return true;
}

void CleanupDeviceD3D(Dx11Window& window)
{
    CleanupRenderTarget(window);

    if (window.swapChain)
    {
        window.swapChain->Release();
        window.swapChain = nullptr;
    }

    if (window.deviceContext)
    {
        window.deviceContext->Release();
        window.deviceContext = nullptr;
    }

    if (window.device)
    {
        window.device->Release();
        window.device = nullptr;
    }
}

void CreateRenderTarget(Dx11Window& window)
{
    ID3D11Texture2D* pBackBuffer = nullptr;

    window.swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

    window.device->CreateRenderTargetView(
        pBackBuffer,
        nullptr,
        &window.mainRenderTargetView
    );

    pBackBuffer->Release();
}

void CleanupRenderTarget(Dx11Window& window)
{
    if (window.mainRenderTargetView)
    {
        window.mainRenderTargetView->Release();
        window.mainRenderTargetView = nullptr;
    }
}

void HandleWindowResize(Dx11Window& window)
{
    if (window.resizeWidth == 0 || window.resizeHeight == 0)
        return;

    CleanupRenderTarget(window);

    window.swapChain->ResizeBuffers(
        0,
        window.resizeWidth,
        window.resizeHeight,
        DXGI_FORMAT_UNKNOWN,
        0
    );

    window.resizeWidth = 0;
    window.resizeHeight = 0;

    CreateRenderTarget(window);
}

bool HandleSwapChainOcclusion(Dx11Window& window)
{
    if (
        window.swapChainOccluded &&
        window.swapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED
    )
    {
        ::Sleep(10);
        return true;
    }

    window.swapChainOccluded = false;
    return false;
}

void SetRenderTarget(Dx11Window& window)
{
    window.deviceContext->OMSetRenderTargets(
        1,
        &window.mainRenderTargetView,
        nullptr
    );
}

void ClearRenderTarget(Dx11Window& window, const float clearColor[4])
{
    window.deviceContext->ClearRenderTargetView(
        window.mainRenderTargetView,
        clearColor
    );
}

HRESULT PresentSwapChain(Dx11Window& window)
{
    HRESULT hr = window.swapChain->Present(1, 0);
    window.swapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    return hr;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;

        if (g_Window)
        {
            g_Window->resizeWidth = static_cast<UINT>(LOWORD(lParam));
            g_Window->resizeHeight = static_cast<UINT>(HIWORD(lParam));
        }

        return 0;

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;

        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
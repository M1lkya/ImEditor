#pragma once

#include <windows.h>
#include <d3d11.h>
#include <tchar.h>

struct Dx11Window
{
    HWND hwnd = nullptr;
    WNDCLASSEXW wc = {};

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* mainRenderTargetView = nullptr;

    bool swapChainOccluded = false;
    UINT resizeWidth = 0;
    UINT resizeHeight = 0;
};

bool CreateDx11Window(
    Dx11Window& window,
    const wchar_t* title,
    int width,
    int height,
    float dpiScale
);

void DestroyDx11Window(Dx11Window& window);

bool CreateDeviceD3D(Dx11Window& window);
void CleanupDeviceD3D(Dx11Window& window);

void CreateRenderTarget(Dx11Window& window);
void CleanupRenderTarget(Dx11Window& window);

void HandleWindowResize(Dx11Window& window);
bool HandleSwapChainOcclusion(Dx11Window& window);

void SetRenderTarget(Dx11Window& window);
void ClearRenderTarget(Dx11Window& window, const float clearColor[4]);

HRESULT PresentSwapChain(Dx11Window& window);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
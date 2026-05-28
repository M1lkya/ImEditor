#pragma once

#include <windows.h>
#include <wrl.h>
#include <wrl/client.h>
#include <WebView2.h>

class MonacoHost
{
public:
    void Initialize(HWND parentHwnd);
    void SetBounds(const RECT& bounds);
    void SetVisible(bool visible);

private:
    HWND m_parentHwnd = nullptr;

    Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controller;
    Microsoft::WRL::ComPtr<ICoreWebView2> m_webview;
};
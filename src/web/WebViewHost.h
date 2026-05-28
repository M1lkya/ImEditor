#pragma once

#include <windows.h>
#include <wrl.h>
#include <wrl/client.h>
#include <WebView2.h>

class WebViewHost
{
public:
    void Initialize(HWND parentHwnd);
    void SetBounds(const RECT& bounds);
    void SetVisible(bool visible);

private:
    HWND m_parentHwnd = nullptr;

    EventRegistrationToken m_webResourceRequestedToken = {};

    Microsoft::WRL::ComPtr<ICoreWebView2Environment> m_environment;
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controller;
    Microsoft::WRL::ComPtr<ICoreWebView2> m_webview;
};
#pragma once

#include "dx11_window.h"
#include "WebViewHost.h"

class App
{
public:
    bool Initialize();
    int Run();
    void Shutdown();

private:
    void BeginFrame();
    void RenderFrame();
    void EndFrame();

private:
    Dx11Window m_window;
    WebViewHost m_webView;
    bool m_done = false;
};
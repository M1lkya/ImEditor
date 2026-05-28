#pragma once

#include "dx11_window.h"
#include "MonacoHost.h"

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
    MonacoHost m_monaco;
    bool m_done = false;
};
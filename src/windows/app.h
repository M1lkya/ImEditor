#pragma once

#include "dx11_window.h"

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
    bool m_done = false;
};
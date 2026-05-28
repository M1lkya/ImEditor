#include "app.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "editor.h"

bool App::Initialize()
{
    ImGui_ImplWin32_EnableDpiAwareness();

    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(
        ::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY)
    );

    if (!CreateDx11Window(m_window, L"ImEditor", 1280, 800, main_scale))
        return false;

    ::ShowWindow(m_window.hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(m_window.hwnd);

    m_monaco.Initialize(m_window.hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    ImGui_ImplWin32_Init(m_window.hwnd);
    ImGui_ImplDX11_Init(m_window.device, m_window.deviceContext);

    return true;
}

int App::Run()
{
    while (!m_done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                m_done = true;
        }

        if (m_done)
            break;

        if (HandleSwapChainOcclusion(m_window))
            continue;

        HandleWindowResize(m_window);

        BeginFrame();
        RenderFrame();
        EndFrame();
    }

    Shutdown();
    return 0;
}

void App::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyDx11Window(m_window);
}

void App::BeginFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void App::RenderFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    ImVec2 editorMin;
    ImVec2 editorMax;

    bool showEditor = DrawEditor(io.DisplaySize, &editorMin, &editorMax);

    if (showEditor)
    {
        RECT rect;
        rect.left = static_cast<LONG>(editorMin.x);
        rect.top = static_cast<LONG>(editorMin.y);
        rect.right = static_cast<LONG>(editorMax.x);
        rect.bottom = static_cast<LONG>(editorMax.y);

        m_monaco.SetBounds(rect);
        m_monaco.SetVisible(true);
    }
    else
    {
        m_monaco.SetVisible(false);
    }
}

void App::EndFrame()
{
    ImGui::Render();

    ImVec4 clear_color = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);

    const float clear_color_with_alpha[4] = {
        clear_color.x * clear_color.w,
        clear_color.y * clear_color.w,
        clear_color.z * clear_color.w,
        clear_color.w
    };

    SetRenderTarget(m_window);
    ClearRenderTarget(m_window, clear_color_with_alpha);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    PresentSwapChain(m_window);
}
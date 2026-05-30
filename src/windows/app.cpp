#include "app.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "editor.h"
#include "AppPaths.h"

#include "embedded_font.h"
#include "embedded_codicon.h"

#include <objbase.h>
#include <shobjidl.h>
#include <wrl/client.h>

#include <string_view>

namespace
{
    using Microsoft::WRL::ComPtr;

    constexpr std::string_view kWebMessageReady = "ready";
    constexpr std::string_view kWebMessageSaveRequested = "saveRequested";
    constexpr std::string_view kWebMessageContentChangedPrefix = "contentChanged\n";

    void LoadImEditorFonts(ImGuiIO& io, float mainScale)
    {
        ImFontConfig fontConfig;
        fontConfig.OversampleH = 3;
        fontConfig.OversampleV = 2;
        fontConfig.PixelSnapH = false;

        fontConfig.FontDataOwnedByAtlas = false;

        ImFont* jetBrainsMono = io.Fonts->AddFontFromMemoryTTF(
            const_cast<unsigned char*>(kJetBrainsMonoRegular),
            static_cast<int>(kJetBrainsMonoRegularSize),
            16.0f * mainScale,
            &fontConfig
        );

        if (jetBrainsMono)
        {
            io.FontDefault = jetBrainsMono;
        }
        else
        {
            io.Fonts->AddFontDefault();
        }

        static const ImWchar codiconRanges[] = {
            0xE000, 0xF8FF,
            0
        };

        ImFontConfig codiconConfig;
        codiconConfig.MergeMode = true;
        codiconConfig.PixelSnapH = true;
        codiconConfig.FontDataOwnedByAtlas = false;
        codiconConfig.GlyphMinAdvanceX = 16.0f * mainScale;

        io.Fonts->AddFontFromMemoryTTF(
            const_cast<unsigned char*>(kCodicon),
            static_cast<int>(kCodiconSize),
            16.0f * mainScale,
            &codiconConfig,
            codiconRanges
        );
    }
}

bool App::Initialize()
{
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    if (FAILED(comHr))
        return false;

    m_comInitialized = true;

    if (!CheckInit(m_paths))
        return false;

    ImGui_ImplWin32_EnableDpiAwareness();

    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(
        ::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY)
    );

    if (!CreateDx11Window(m_window, L"ImEditor", 1280, 800, main_scale))
        return false;

    ::ShowWindow(m_window.hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(m_window.hwnd);

    m_webView.SetMessageHandler(
        [this](const std::string& message)
        {
            HandleWebViewMessage(message);
        }
    );

    m_webView.Initialize(
        m_window.hwnd,
        m_paths.webView2Folder
    );

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    m_imguiIniPath = PathToUtf8(m_paths.imguiIniFile);
    io.IniFilename = m_imguiIniPath.c_str();

    LoadImEditorFonts(io, main_scale);

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
    m_webView.Shutdown();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyDx11Window(m_window);

    if (m_comInitialized)
    {
        CoUninitialize();
        m_comInitialized = false;
    }
}

void App::BeginFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void App::RenderFrame()
{
    MaybeAutoRefreshWorkspace(
        m_editorState,
        ImGui::GetTime()
    );

    ImGuiIO& io = ImGui::GetIO();

    ImVec2 editorMin;
    ImVec2 editorMax;

    EditorUiIntent intent;
    ResetEditorUiIntent(intent);

    bool hasEditorRect = DrawEditor(
        m_editorState,
        io.DisplaySize,
        &editorMin,
        &editorMax,
        &intent
    );

    HandleEditorIntent(intent);
    SyncActiveFileToWebView();

    if (hasEditorRect && HasActiveFile(m_editorState))
    {
        RECT rect;
        rect.left = static_cast<LONG>(editorMin.x);
        rect.top = static_cast<LONG>(editorMin.y);
        rect.right = static_cast<LONG>(editorMax.x);
        rect.bottom = static_cast<LONG>(editorMax.y);

        m_webView.SetBounds(rect);
        m_webView.SetVisible(true);
    }
    else
    {
        m_webView.SetVisible(false);
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

void App::HandleEditorIntent(const EditorUiIntent& intent)
{
    if (intent.openWorkspaceRequested && !m_editorState.hasWorkspace)
    {
        std::filesystem::path folder = PickWorkspaceFolder();

        if (!folder.empty())
            OpenWorkspace(m_editorState, folder);
    }

    if (
        intent.toggleWorkspaceEntryIndex >= 0 &&
        intent.toggleWorkspaceEntryIndex <
            static_cast<int>(m_editorState.workspaceEntries.size())
    )
    {
        const WorkspaceEntry& entry =
            m_editorState.workspaceEntries[intent.toggleWorkspaceEntryIndex];

        if (entry.isDirectory)
            ToggleDirectoryExpanded(m_editorState, entry.absolutePath);
    }

    if (
        intent.openWorkspaceEntryIndex >= 0 &&
        intent.openWorkspaceEntryIndex <
            static_cast<int>(m_editorState.workspaceEntries.size())
    )
    {
        const WorkspaceEntry& entry =
            m_editorState.workspaceEntries[intent.openWorkspaceEntryIndex];

        if (!entry.isDirectory)
            OpenFileInEditor(m_editorState, entry.absolutePath);
    }

    if (intent.selectTabIndex >= 0)
    {
        SetActiveOpenFile(m_editorState, intent.selectTabIndex);
    }

    if (intent.saveActiveFileRequested)
    {
        SaveActiveFile(m_editorState);
    }

    if (intent.closeTabIndex >= 0)
    {
        CloseOpenFile(m_editorState, intent.closeTabIndex);
    }
}

void App::HandleWebViewMessage(const std::string& message)
{
    std::string_view view(message);

    if (view == kWebMessageReady)
    {
        m_webViewReady = true;
        m_editorState.activeFileNeedsWebSync = true;
        return;
    }

    if (view == kWebMessageSaveRequested)
    {
        SaveActiveFile(m_editorState);
        return;
    }

    if (view.rfind(kWebMessageContentChangedPrefix, 0) == 0)
    {
        std::string content(
            view.substr(kWebMessageContentChangedPrefix.size())
        );

        SetActiveFileContent(m_editorState, content);
    }
}

void App::SyncActiveFileToWebView()
{
    if (!m_webViewReady || !m_editorState.activeFileNeedsWebSync)
        return;

    const EditorOpenFile* file = GetActiveOpenFile(m_editorState);

    if (!file)
    {
        if (m_webView.PostStringMessage("clearEditor"))
            m_editorState.activeFileNeedsWebSync = false;

        return;
    }

    std::string message;
    message.reserve(file->content.size() + file->displayPath.size() + 16);

    message += "setFile\n";
    message += file->displayPath;
    message += "\n";
    message += file->content;

    if (m_webView.PostStringMessage(message))
        m_editorState.activeFileNeedsWebSync = false;
}

std::filesystem::path App::PickWorkspaceFolder()
{
    ComPtr<IFileOpenDialog> dialog;

    HRESULT hr = CoCreateInstance(
        CLSID_FileOpenDialog,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&dialog)
    );

    if (FAILED(hr) || !dialog)
        return {};

    DWORD options = 0;
    dialog->GetOptions(&options);

    dialog->SetOptions(
        options |
        FOS_PICKFOLDERS |
        FOS_FORCEFILESYSTEM |
        FOS_PATHMUSTEXIST
    );

    dialog->SetTitle(L"Open Workspace Folder");

    hr = dialog->Show(m_window.hwnd);

    if (FAILED(hr))
        return {};

    ComPtr<IShellItem> item;
    hr = dialog->GetResult(&item);

    if (FAILED(hr) || !item)
        return {};

    PWSTR rawPath = nullptr;
    hr = item->GetDisplayName(SIGDN_FILESYSPATH, &rawPath);

    if (FAILED(hr) || !rawPath)
        return {};

    std::filesystem::path result(rawPath);
    CoTaskMemFree(rawPath);

    return result;
}
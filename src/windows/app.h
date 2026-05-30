#pragma once

#include "dx11_window.h"
#include "WebViewHost.h"
#include "AppPaths.h"
#include "EditorState.h"

#include <filesystem>
#include <string>

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

    void HandleEditorIntent(const EditorUiIntent& intent);
    void HandleWebViewMessage(const std::string& message);
    void SyncActiveFileToWebView();

    std::filesystem::path PickWorkspaceFolder();

private:
    Dx11Window m_window;
    WebViewHost m_webView;

    AppPaths m_paths;
    EditorState m_editorState;

    std::string m_imguiIniPath;

    bool m_done = false;
    bool m_webViewReady = false;
    bool m_comInitialized = false;
};
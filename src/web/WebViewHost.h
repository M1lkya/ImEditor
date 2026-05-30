#pragma once

#include <windows.h>
#include <wrl.h>
#include <wrl/client.h>
#include <WebView2.h>

#include <filesystem>
#include <functional>
#include <string>

class WebViewHost
{
public:
    using MessageHandler = std::function<void(const std::string&)>;

    void SetMessageHandler(MessageHandler handler);

    void Initialize(HWND parentHwnd, const std::filesystem::path& userDataFolder);
    void Shutdown();

    void SetBounds(const RECT& bounds);
    void SetVisible(bool visible);

    bool PostStringMessage(const std::string& message);

private:
    HWND m_parentHwnd = nullptr;

    EventRegistrationToken m_webResourceRequestedToken = {};
    EventRegistrationToken m_webMessageReceivedToken = {};
    EventRegistrationToken m_acceleratorKeyPressedToken = {};

    MessageHandler m_messageHandler;

    Microsoft::WRL::ComPtr<ICoreWebView2Environment> m_environment;
    Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controller;
    Microsoft::WRL::ComPtr<ICoreWebView2> m_webview;
};
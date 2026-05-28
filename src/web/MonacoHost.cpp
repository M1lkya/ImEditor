#include "MonacoHost.h"
#include "html.h"

#include <filesystem>

using Microsoft::WRL::Callback;

void MonacoHost::Initialize(HWND parentHwnd)
{
    m_parentHwnd = parentHwnd;

    CreateCoreWebView2EnvironmentWithOptions(
        nullptr,
        nullptr,
        nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT
            {
                if (FAILED(result) || !env)
                    return result;

                env->CreateCoreWebView2Controller(
                    m_parentHwnd,
                    Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [this](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT
                        {
                            if (FAILED(result) || !controller)
                                return result;

                            m_controller = controller;

                            HRESULT hr = m_controller->get_CoreWebView2(
                                m_webview.GetAddressOf()
                            );

                            if (FAILED(hr) || !m_webview)
                                return hr;

                            // Make WebView2's background transparent so CSS border-radius looks correct.
                            Microsoft::WRL::ComPtr<ICoreWebView2Controller2> controller2;

                            if (SUCCEEDED(m_controller.As(&controller2)) && controller2)
                            {
                                COREWEBVIEW2_COLOR transparent = {};
                                transparent.A = 0;
                                transparent.R = 0;
                                transparent.G = 0;
                                transparent.B = 0;

                                controller2->put_DefaultBackgroundColor(transparent);
                            }

                            // Enable JavaScript and web messages.
                            Microsoft::WRL::ComPtr<ICoreWebView2Settings> settings;

                            hr = m_webview->get_Settings(settings.GetAddressOf());

                            if (SUCCEEDED(hr) && settings)
                            {
                                settings->put_IsScriptEnabled(TRUE);
                                settings->put_IsWebMessageEnabled(TRUE);
                            }

                            // Map ./external to https://imeditor-assets.example/
                            // So external/monaco/vs/loader.js becomes:
                            // https://imeditor-assets.example/monaco/vs/loader.js
                            auto assetsPath =
                                std::filesystem::absolute("external").wstring();

                            Microsoft::WRL::ComPtr<ICoreWebView2_3> webview3;

                            if (SUCCEEDED(m_webview.As(&webview3)) && webview3)
                            {
                                webview3->SetVirtualHostNameToFolderMapping(
                                    L"imeditor-assets.example",
                                    assetsPath.c_str(),
                                    COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_DENY
                                );
                            }

                            m_webview->NavigateToString(MONACO_HTML);

                            return S_OK;
                        }
                    ).Get()
                );

                return S_OK;
            }
        ).Get()
    );
}

void MonacoHost::SetBounds(const RECT& bounds)
{
    if (m_controller)
        m_controller->put_Bounds(bounds);
}

void MonacoHost::SetVisible(bool visible)
{
    if (m_controller)
        m_controller->put_IsVisible(visible ? TRUE : FALSE);
}
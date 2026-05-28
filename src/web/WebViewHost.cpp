#include "WebViewHost.h"
#include "embedded_frontend.h"

#include <string>
#include <shlwapi.h>

using Microsoft::WRL::Callback;

static const EmbeddedAsset* FindEmbeddedAsset(const std::wstring& path)
{
    for (size_t i = 0; i < kEmbeddedAssetCount; ++i)
    {
        if (path == kEmbeddedAssets[i].path)
            return &kEmbeddedAssets[i];
    }

    return nullptr;
}

static std::wstring GetPathFromUri(const std::wstring& uri)
{
    const std::wstring prefix = L"https://imeditor-assets.example";

    if (uri.rfind(prefix, 0) != 0)
        return L"/index.html";

    std::wstring path = uri.substr(prefix.size());

    size_t queryPos = path.find_first_of(L"?#");
    if (queryPos != std::wstring::npos)
        path.resize(queryPos);

    if (path.empty() || path == L"/")
        path = L"/index.html";

    return path;
}

void WebViewHost::Initialize(HWND parentHwnd)
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

                m_environment = env;

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

                            Microsoft::WRL::ComPtr<ICoreWebView2Settings> settings;

                            hr = m_webview->get_Settings(settings.GetAddressOf());

                            if (SUCCEEDED(hr) && settings)
                            {
                                settings->put_IsScriptEnabled(TRUE);
                                settings->put_IsWebMessageEnabled(TRUE);
                            }

                            m_webview->AddWebResourceRequestedFilter(
                                L"https://imeditor-assets.example/*",
                                COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL
                            );

                            m_webview->add_WebResourceRequested(
                                Callback<ICoreWebView2WebResourceRequestedEventHandler>(
                                    [this](
                                        ICoreWebView2* sender,
                                        ICoreWebView2WebResourceRequestedEventArgs* args
                                    ) -> HRESULT
                                    {
                                        (void)sender;

                                        Microsoft::WRL::ComPtr<ICoreWebView2WebResourceRequest> request;
                                        args->get_Request(request.GetAddressOf());

                                        if (!request)
                                            return S_OK;

                                        LPWSTR rawUri = nullptr;
                                        HRESULT requestHr = request->get_Uri(&rawUri);

                                        if (FAILED(requestHr))
                                            return requestHr;

                                        std::wstring uri = rawUri ? rawUri : L"";

                                        if (rawUri)
                                            CoTaskMemFree(rawUri);

                                        std::wstring path = GetPathFromUri(uri);
                                        const EmbeddedAsset* asset = FindEmbeddedAsset(path);

                                        if (!asset)
                                            return S_OK;

                                        IStream* stream = SHCreateMemStream(
                                            asset->data,
                                            static_cast<UINT>(asset->size)
                                        );

                                        if (!stream)
                                            return E_FAIL;

                                        std::wstring headers =
                                            L"Content-Type: " + std::wstring(asset->mime);

                                        Microsoft::WRL::ComPtr<ICoreWebView2WebResourceResponse> response;

                                        HRESULT responseHr = m_environment->CreateWebResourceResponse(
                                            stream,
                                            200,
                                            L"OK",
                                            headers.c_str(),
                                            response.GetAddressOf()
                                        );

                                        stream->Release();

                                        if (FAILED(responseHr))
                                            return responseHr;

                                        args->put_Response(response.Get());

                                        return S_OK;
                                    }
                                ).Get(),
                                &m_webResourceRequestedToken
                            );

                            m_webview->Navigate(
                                L"https://imeditor-assets.example/index.html"
                            );

                            return S_OK;
                        }
                    ).Get()
                );

                return S_OK;
            }
        ).Get()
    );
}

void WebViewHost::SetBounds(const RECT& bounds)
{
    if (m_controller)
        m_controller->put_Bounds(bounds);
}

void WebViewHost::SetVisible(bool visible)
{
    if (m_controller)
        m_controller->put_IsVisible(visible ? TRUE : FALSE);
}
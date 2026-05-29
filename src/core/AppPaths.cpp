#include "AppPaths.h"

#include <windows.h>
#include <shlobj.h>

namespace
{
    std::filesystem::path GetKnownFolder(REFKNOWNFOLDERID folderId)
    {
        PWSTR rawPath = nullptr;

        HRESULT hr = SHGetKnownFolderPath(
            folderId,
            KF_FLAG_CREATE,
            nullptr,
            &rawPath
        );

        if (FAILED(hr) || !rawPath)
            return {};

        std::filesystem::path result(rawPath);
        CoTaskMemFree(rawPath);

        return result;
    }
}

std::string PathToUtf8(const std::filesystem::path& path)
{
    std::wstring wide = path.wstring();

    if (wide.empty())
        return {};

    int size = WideCharToMultiByte(
        CP_UTF8,
        0,
        wide.c_str(),
        -1,
        nullptr,
        0,
        nullptr,
        nullptr
    );

    if (size <= 0)
        return {};

    std::string result;
    result.resize(static_cast<size_t>(size - 1));

    WideCharToMultiByte(
        CP_UTF8,
        0,
        wide.c_str(),
        -1,
        result.data(),
        size,
        nullptr,
        nullptr
    );

    return result;
}

bool CheckInit(AppPaths& outPaths)
{
    try
    {
        outPaths.localAppData = GetKnownFolder(FOLDERID_LocalAppData);

        if (outPaths.localAppData.empty())
            return false;

        outPaths.root = outPaths.localAppData / "ImEditor";

        outPaths.imguiIniFile = outPaths.root / "imgui.ini";
        outPaths.webView2Folder = outPaths.root / "WebView2";

        outPaths.pluginsFolder = outPaths.root / "plugins";
        outPaths.themesFolder = outPaths.root / "themes";
        outPaths.logsFolder = outPaths.root / "logs";

        std::filesystem::create_directories(outPaths.root);
        std::filesystem::create_directories(outPaths.webView2Folder);
        std::filesystem::create_directories(outPaths.pluginsFolder);
        std::filesystem::create_directories(outPaths.themesFolder);
        std::filesystem::create_directories(outPaths.logsFolder);

        return true;
    }
    catch (...)
    {
        return false;
    }
}
#pragma once

#include <filesystem>
#include <string>

struct AppPaths{
    std::filesystem::path localAppData;
    std::filesystem::path root;

    std::filesystem::path imguiIniFile;
    std::filesystem::path webView2Folder;

    std::filesystem::path pluginsFolder;
    std::filesystem::path themesFolder;
    std::filesystem::path logsFolder;
};

bool CheckInit(AppPaths& outPaths);
std::string PathToUtf8(const std::filesystem::path& path);
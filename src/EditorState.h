#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

enum class ExplorerPage
{
    Files,
    SourceControl,
    Extensions,
    Settings
};

enum class ActiveContentPage
{
    None,
    Welcome,
    Settings,
    Editor
};

struct WorkspaceEntry
{
    std::filesystem::path absolutePath;
    std::filesystem::path relativePath;

    std::string name;
    std::string displayPath;

    bool isDirectory = false;
    int depth = 0;
};

struct EditorOpenFile
{
    std::filesystem::path absolutePath;
    std::filesystem::path relativePath;

    std::string title;
    std::string displayPath;
    std::string content;

    bool dirty = false;
};

struct EditorUiIntent
{
    bool openWorkspaceRequested = false;
    bool saveActiveFileRequested = false;

    bool selectWelcomeTabRequested = false;
    bool closeWelcomeTabRequested = false;

    int openWorkspaceEntryIndex = -1;
    int toggleWorkspaceEntryIndex = -1;
    int selectTabIndex = -1;
    int closeTabIndex = -1;
};

struct EditorState
{
    ExplorerPage activePage = ExplorerPage::Files;
    ActiveContentPage activeContentPage = ActiveContentPage::Welcome;

    bool explorerOpen = false;

    bool welcomeTabOpen = true;

    float explorerWidth = 220.0f;
    float tabScrollX = 0.0f;

    std::filesystem::path workspaceRoot;
    bool hasWorkspace = false;

    std::vector<std::filesystem::path> expandedDirectories;
    std::vector<WorkspaceEntry> workspaceEntries;

    std::vector<EditorOpenFile> openFiles;
    int activeFileIndex = -1;

    bool activeFileNeedsWebSync = true;

    double nextWorkspaceRefreshTime = 0.0;
    double workspaceRefreshIntervalSeconds = 8.0;

    std::string statusMessage = "Open a workspace folder to start.";
};

void ResetEditorUiIntent(EditorUiIntent& intent);

bool OpenWorkspace(EditorState& state, const std::filesystem::path& folder);
bool RefreshWorkspace(EditorState& state);
bool MaybeAutoRefreshWorkspace(EditorState& state, double currentTimeSeconds);

bool IsDirectoryExpanded(const EditorState& state, const std::filesystem::path& folder);
void SetDirectoryExpanded(EditorState& state, const std::filesystem::path& folder, bool expanded);
void ToggleDirectoryExpanded(EditorState& state, const std::filesystem::path& folder);

int OpenFileInEditor(EditorState& state, const std::filesystem::path& filePath);
void SetActiveOpenFile(EditorState& state, int index);
void CloseOpenFile(EditorState& state, int index);

EditorOpenFile* GetActiveOpenFile(EditorState& state);
const EditorOpenFile* GetActiveOpenFile(const EditorState& state);
bool HasActiveFile(const EditorState& state);

void SetActiveFileContent(EditorState& state, const std::string& content);
bool SaveActiveFile(EditorState& state);

std::string PathToDisplayString(const std::filesystem::path& path);
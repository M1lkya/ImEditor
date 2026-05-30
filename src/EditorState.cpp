#include "EditorState.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <system_error>

namespace
{
    constexpr std::uintmax_t kMaxOpenFileBytes = 256ULL * 1024ULL * 1024ULL;

    constexpr std::size_t kMaxVisibleWorkspaceEntries = 12000;
    constexpr std::size_t kMaxFilesPerDirectory = 5000;

    std::string ToLower(std::string value)
    {
        std::transform(
            value.begin(),
            value.end(),
            value.begin(),
            [](unsigned char c)
            {
                return static_cast<char>(std::tolower(c));
            }
        );

        return value;
    }

    std::filesystem::path NormalizePath(const std::filesystem::path& path)
    {
        std::error_code ec;
        std::filesystem::path normalized = std::filesystem::weakly_canonical(path, ec);

        if (ec)
            return std::filesystem::absolute(path);

        return normalized;
    }

    bool SamePath(
        const std::filesystem::path& a,
        const std::filesystem::path& b
    )
    {
        std::error_code ec;

        if (std::filesystem::equivalent(a, b, ec) && !ec)
            return true;

        return NormalizePath(a) == NormalizePath(b);
    }

    std::string ReadWholeFile(const std::filesystem::path& path, bool& ok)
    {
        ok = false;

        std::ifstream file(path, std::ios::binary);

        if (!file)
            return {};

        std::ostringstream stream;
        stream << file.rdbuf();

        std::string content = stream.str();

        if (content.find('\0') != std::string::npos)
            return {};

        ok = true;
        return content;
    }

    bool WriteWholeFile(
        const std::filesystem::path& path,
        const std::string& content
    )
    {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);

        if (!file)
            return false;

        file.write(content.data(), static_cast<std::streamsize>(content.size()));
        return file.good();
    }

    std::filesystem::path MakeRelativePath(
        const std::filesystem::path& root,
        const std::filesystem::path& path
    )
    {
        std::error_code ec;
        std::filesystem::path relative = std::filesystem::relative(path, root, ec);

        if (ec)
            return path.filename();

        return relative;
    }

    WorkspaceEntry MakeWorkspaceEntry(
        const std::filesystem::path& root,
        const std::filesystem::path& path,
        bool isDirectory,
        int depth
    )
    {
        WorkspaceEntry entry;
        entry.absolutePath = NormalizePath(path);
        entry.relativePath = MakeRelativePath(root, entry.absolutePath);
        entry.name = PathToDisplayString(path.filename());
        entry.displayPath = PathToDisplayString(entry.relativePath);
        entry.isDirectory = isDirectory;
        entry.depth = depth;

        return entry;
    }

    void CollectDirectoryEntries(
        const std::filesystem::path& directory,
        std::vector<std::filesystem::directory_entry>& outDirectories,
        std::vector<std::filesystem::directory_entry>& outFiles,
        bool& hitFileLimit
    )
    {
        hitFileLimit = false;

        std::error_code ec;

        for (const auto& entry : std::filesystem::directory_iterator(directory, ec))
        {
            if (ec)
                break;

            std::error_code typeEc;
            bool isDirectory = entry.is_directory(typeEc);
            bool isRegularFile = entry.is_regular_file(typeEc);

            if (typeEc)
                continue;

            std::string name = PathToDisplayString(entry.path().filename());

            if (name.empty())
                continue;

            if (isDirectory)
            {
                outDirectories.push_back(entry);
                continue;
            }

            if (isRegularFile)
            {
                if (outFiles.size() < kMaxFilesPerDirectory)
                {
                    outFiles.push_back(entry);
                }
                else
                {
                    hitFileLimit = true;
                }
            }
        }
    }

    void SortEntries(std::vector<std::filesystem::directory_entry>& entries)
    {
        std::sort(
            entries.begin(),
            entries.end(),
            [](const auto& a, const auto& b)
            {
                return ToLower(PathToDisplayString(a.path().filename())) <
                    ToLower(PathToDisplayString(b.path().filename()));
            }
        );
    }

    bool CanAddVisibleEntry(EditorState& state)
    {
        if (state.workspaceEntries.size() < kMaxVisibleWorkspaceEntries)
            return true;

        state.statusMessage =
            "Workspace is large. Showing the first visible entries only.";

        return false;
    }

    void AddDirectoryEntries(
        EditorState& state,
        const std::filesystem::path& root,
        const std::filesystem::path& directory,
        int depth
    )
    {
        if (!CanAddVisibleEntry(state))
            return;

        bool hitFileLimit = false;

        std::vector<std::filesystem::directory_entry> directories;
        std::vector<std::filesystem::directory_entry> files;

        CollectDirectoryEntries(
            directory,
            directories,
            files,
            hitFileLimit
        );

        SortEntries(directories);
        SortEntries(files);

        for (const auto& entry : directories)
        {
            if (!CanAddVisibleEntry(state))
                return;

            std::filesystem::path entryPath = NormalizePath(entry.path());

            state.workspaceEntries.push_back(
                MakeWorkspaceEntry(root, entryPath, true, depth)
            );

            if (IsDirectoryExpanded(state, entryPath))
            {
                AddDirectoryEntries(
                    state,
                    root,
                    entryPath,
                    depth + 1
                );
            }
        }

        for (const auto& entry : files)
        {
            if (!CanAddVisibleEntry(state))
                return;

            std::filesystem::path entryPath = NormalizePath(entry.path());

            state.workspaceEntries.push_back(
                MakeWorkspaceEntry(root, entryPath, false, depth)
            );
        }

        if (hitFileLimit)
        {
            state.statusMessage =
                "Folder is large. Showing folders and the first files only.";
        }
    }

    bool RebuildWorkspaceEntries(EditorState& state)
    {
        if (!state.hasWorkspace)
            return false;

        std::error_code ec;

        if (
            !std::filesystem::exists(state.workspaceRoot, ec) ||
            !std::filesystem::is_directory(state.workspaceRoot, ec)
        )
        {
            state.workspaceEntries.clear();
            state.hasWorkspace = false;
            state.statusMessage = "Workspace folder no longer exists.";
            return false;
        }

        std::string oldStatus = state.statusMessage;

        state.workspaceEntries.clear();

        AddDirectoryEntries(
            state,
            state.workspaceRoot,
            state.workspaceRoot,
            0
        );

        if (
            state.statusMessage == "Folder is large. Showing folders and the first files only." ||
            state.statusMessage == "Workspace is large. Showing the first visible entries only."
        )
        {
            return true;
        }

        state.statusMessage = oldStatus;
        return true;
    }
}

void ResetEditorUiIntent(EditorUiIntent& intent)
{
    intent = {};
}

std::string PathToDisplayString(const std::filesystem::path& path)
{
    std::u8string u8 = path.generic_u8string();

    return std::string(
        reinterpret_cast<const char*>(u8.c_str()),
        u8.size()
    );
}

bool OpenWorkspace(EditorState& state, const std::filesystem::path& folder)
{
    std::error_code ec;

    if (!std::filesystem::exists(folder, ec) || !std::filesystem::is_directory(folder, ec))
    {
        state.statusMessage = "That workspace folder could not be opened.";
        return false;
    }

    state.workspaceRoot = NormalizePath(folder);
    state.hasWorkspace = true;

    state.expandedDirectories.clear();
    state.expandedDirectories.push_back(state.workspaceRoot);

    state.workspaceEntries.clear();

    state.openFiles.clear();
    state.activeFileIndex = -1;
    state.activeFileNeedsWebSync = true;

    state.statusMessage =
        "Opened workspace: " + PathToDisplayString(state.workspaceRoot);

    RebuildWorkspaceEntries(state);

    state.nextWorkspaceRefreshTime = 0.0;

    return true;
}

bool RefreshWorkspace(EditorState& state)
{
    if (!state.hasWorkspace)
        return false;

    return RebuildWorkspaceEntries(state);
}

bool MaybeAutoRefreshWorkspace(EditorState& state, double currentTimeSeconds)
{
    if (!state.hasWorkspace)
        return false;

    if (currentTimeSeconds < state.nextWorkspaceRefreshTime)
        return false;

    state.nextWorkspaceRefreshTime =
        currentTimeSeconds + state.workspaceRefreshIntervalSeconds;

    return RefreshWorkspace(state);
}

bool IsDirectoryExpanded(const EditorState& state, const std::filesystem::path& folder)
{
    std::filesystem::path normalized = NormalizePath(folder);

    for (const std::filesystem::path& expandedPath : state.expandedDirectories)
    {
        if (SamePath(expandedPath, normalized))
            return true;
    }

    return false;
}

void SetDirectoryExpanded(
    EditorState& state,
    const std::filesystem::path& folder,
    bool expanded
)
{
    std::filesystem::path normalized = NormalizePath(folder);

    auto it = std::find_if(
        state.expandedDirectories.begin(),
        state.expandedDirectories.end(),
        [&](const std::filesystem::path& existing)
        {
            return SamePath(existing, normalized);
        }
    );

    if (expanded)
    {
        if (it == state.expandedDirectories.end())
            state.expandedDirectories.push_back(normalized);

        return;
    }

    if (it != state.expandedDirectories.end())
        state.expandedDirectories.erase(it);
}

void ToggleDirectoryExpanded(EditorState& state, const std::filesystem::path& folder)
{
    bool expanded = IsDirectoryExpanded(state, folder);

    SetDirectoryExpanded(
        state,
        folder,
        !expanded
    );

    RefreshWorkspace(state);
}

int OpenFileInEditor(EditorState& state, const std::filesystem::path& filePath)
{
    std::filesystem::path normalized = NormalizePath(filePath);

    std::error_code ec;

    if (!std::filesystem::exists(normalized, ec) ||
        !std::filesystem::is_regular_file(normalized, ec))
    {
        state.statusMessage = "That file could not be opened.";
        return -1;
    }

    std::uintmax_t size = std::filesystem::file_size(normalized, ec);

    if (!ec && size > kMaxOpenFileBytes)
    {
        state.statusMessage = "That file is too large to open in the editor right now.";
        return -1;
    }

    for (int i = 0; i < static_cast<int>(state.openFiles.size()); ++i)
    {
        if (SamePath(state.openFiles[i].absolutePath, normalized))
        {
            SetActiveOpenFile(state, i);
            return i;
        }
    }

    bool ok = false;
    std::string content = ReadWholeFile(normalized, ok);

    if (!ok)
    {
        state.statusMessage =
            "Could not read that file as text. Binary files are not supported.";
        return -1;
    }

    EditorOpenFile openFile;
    openFile.absolutePath = normalized;

    if (state.hasWorkspace)
        openFile.relativePath = MakeRelativePath(state.workspaceRoot, normalized);
    else
        openFile.relativePath = normalized.filename();

    openFile.title = PathToDisplayString(normalized.filename());
    openFile.displayPath = PathToDisplayString(openFile.relativePath);
    openFile.content = content;
    openFile.dirty = false;

    state.openFiles.push_back(std::move(openFile));
    state.activeFileIndex = static_cast<int>(state.openFiles.size()) - 1;
    state.activeFileNeedsWebSync = true;

    state.statusMessage =
        "Opened file: " + state.openFiles[state.activeFileIndex].displayPath;

    return state.activeFileIndex;
}

void SetActiveOpenFile(EditorState& state, int index)
{
    if (index < 0 || index >= static_cast<int>(state.openFiles.size()))
        return;

    if (state.activeFileIndex == index)
        return;

    state.activeFileIndex = index;
    state.activeFileNeedsWebSync = true;

    state.statusMessage =
        "Active file: " + state.openFiles[index].displayPath;
}

void CloseOpenFile(EditorState& state, int index)
{
    if (index < 0 || index >= static_cast<int>(state.openFiles.size()))
        return;

    if (state.openFiles[index].dirty)
    {
        state.statusMessage =
            "Save the file before closing it: " + state.openFiles[index].displayPath;
        return;
    }

    bool wasActive = index == state.activeFileIndex;

    state.openFiles.erase(state.openFiles.begin() + index);

    if (state.openFiles.empty())
    {
        state.activeFileIndex = -1;
        state.activeFileNeedsWebSync = true;
        state.statusMessage = "Closed all tabs.";
        return;
    }

    if (state.activeFileIndex > index)
        state.activeFileIndex--;

    if (wasActive)
    {
        if (index >= static_cast<int>(state.openFiles.size()))
            state.activeFileIndex = static_cast<int>(state.openFiles.size()) - 1;
        else
            state.activeFileIndex = index;

        state.activeFileNeedsWebSync = true;
    }

    state.statusMessage = "Closed tab.";
}

EditorOpenFile* GetActiveOpenFile(EditorState& state)
{
    if (!HasActiveFile(state))
        return nullptr;

    return &state.openFiles[state.activeFileIndex];
}

const EditorOpenFile* GetActiveOpenFile(const EditorState& state)
{
    if (!HasActiveFile(state))
        return nullptr;

    return &state.openFiles[state.activeFileIndex];
}

bool HasActiveFile(const EditorState& state)
{
    return
        state.activeFileIndex >= 0 &&
        state.activeFileIndex < static_cast<int>(state.openFiles.size());
}

void SetActiveFileContent(EditorState& state, const std::string& content)
{
    EditorOpenFile* file = GetActiveOpenFile(state);

    if (!file)
        return;

    if (file->content == content)
        return;

    file->content = content;
    file->dirty = true;

    state.statusMessage = "Unsaved changes: " + file->displayPath;
}

bool SaveActiveFile(EditorState& state)
{
    EditorOpenFile* file = GetActiveOpenFile(state);

    if (!file)
    {
        state.statusMessage = "No active file to save.";
        return false;
    }

    if (!WriteWholeFile(file->absolutePath, file->content))
    {
        state.statusMessage = "Failed to save: " + file->displayPath;
        return false;
    }

    file->dirty = false;
    state.statusMessage = "Saved: " + file->displayPath;

    return true;
}
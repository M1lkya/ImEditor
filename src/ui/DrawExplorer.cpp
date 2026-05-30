#include "ui.h"

#include <string>

namespace
{
    constexpr float kHeaderHeight = 52.0f;
    constexpr float kHeaderLeftPadding = 16.0f;
    constexpr float kHeaderTopPadding = 13.0f;

    constexpr float kTreeTopPadding = 6.0f;
    constexpr float kTreeLeftPadding = 10.0f;
    constexpr float kTreeRowHeight = 23.0f;
    constexpr float kTreeIndent = 14.0f;

    ImU32 HeaderTextColor()
    {
        return IM_COL32(238, 238, 238, 255);
    }

    ImU32 MutedTextColor()
    {
        return IM_COL32(140, 140, 140, 255);
    }

    ImU32 NormalTextColor()
    {
        return IM_COL32(205, 205, 205, 255);
    }

    ImU32 BrightTextColor()
    {
        return IM_COL32(245, 245, 245, 255);
    }

    ImU32 HoverFillColor()
    {
        return IM_COL32(255, 255, 255, 13);
    }

    ImU32 SelectedFillColor()
    {
        return IM_COL32(167, 139, 250, 32);
    }

    ImU32 AccentColor()
    {
        return IM_COL32(167, 139, 250, 240);
    }

    ImU32 SeparatorColor()
    {
        return IM_COL32(65, 65, 65, 170);
    }

    const char* GetExplorerTitle(ExplorerPage page)
    {
        switch (page)
        {
        case ExplorerPage::Files:
            return "Files";

        case ExplorerPage::SourceControl:
            return "Source Control";

        case ExplorerPage::Extensions:
            return "Extensions";

        case ExplorerPage::Settings:
            return "Settings";
        }

        return "Explorer";
    }

    const char* GetPlaceholderText(ExplorerPage page)
    {
        switch (page)
        {
        case ExplorerPage::Files:
            return "Open a workspace folder to browse files.";

        case ExplorerPage::SourceControl:
            return "Source control changes will go here later.";

        case ExplorerPage::Extensions:
            return "Extensions will go here later.";

        case ExplorerPage::Settings:
            return "Settings will go here later.";
        }

        return "";
    }

    std::string GetWorkspaceDisplayName(const EditorState& state)
    {
        std::string workspaceName =
            PathToDisplayString(state.workspaceRoot.filename());

        if (workspaceName.empty())
            workspaceName = PathToDisplayString(state.workspaceRoot);

        return workspaceName;
    }

    void DrawFixedExplorerHeader(
        const ImVec2& panelMin,
        const ImVec2& panelMax,
        const std::string& title,
        const char* subtitle
    )
    {
        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImFont* font = ImGui::GetFont();

        draw->AddText(
            font,
            ImGui::GetFontSize() * 1.18f,
            ImVec2(
                panelMin.x + kHeaderLeftPadding,
                panelMin.y + kHeaderTopPadding
            ),
            HeaderTextColor(),
            title.c_str()
        );

        if (subtitle && subtitle[0])
        {
            draw->AddText(
                font,
                ImGui::GetFontSize() * 0.82f,
                ImVec2(
                    panelMin.x + kHeaderLeftPadding,
                    panelMin.y + kHeaderTopPadding + 23.0f
                ),
                MutedTextColor(),
                subtitle
            );
        }

        float separatorY = panelMin.y + kHeaderHeight;

        draw->AddLine(
            ImVec2(panelMin.x + 10.0f, separatorY),
            ImVec2(panelMax.x - 10.0f, separatorY),
            SeparatorColor(),
            1.0f
        );
    }

    void DrawOpenFolderRow(EditorUiIntent* intent)
    {
        ImGui::SetCursorPosY(kTreeTopPadding);

        float width = ImGui::GetWindowSize().x;

        if (width < 1.0f)
            width = 1.0f;

        ImGui::InvisibleButton(
            "OpenFolderRow",
            ImVec2(width, 32.0f)
        );

        bool hovered = ImGui::IsItemHovered();
        bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);

        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();

        ImDrawList* draw = ImGui::GetWindowDrawList();

        if (hovered)
        {
            draw->AddRectFilled(
                ImVec2(min.x + 6.0f, min.y + 2.0f),
                ImVec2(max.x - 6.0f, max.y - 2.0f),
                HoverFillColor(),
                5.0f
            );
        }

        draw->AddText(
            ImVec2(min.x + kTreeLeftPadding + 6.0f, min.y + 8.0f),
            hovered ? BrightTextColor() : NormalTextColor(),
            "Open Folder"
        );

        if (clicked && intent)
            intent->openWorkspaceRequested = true;
    }

    bool IsEntrySelected(const EditorState& state, const WorkspaceEntry& entry)
    {
        if (entry.isDirectory)
            return false;

        const EditorOpenFile* activeFile = GetActiveOpenFile(state);

        if (!activeFile)
            return false;

        return activeFile->absolutePath == entry.absolutePath;
    }

    void DrawTreeEntry(
        const EditorState& state,
        EditorUiIntent* intent,
        int index,
        const WorkspaceEntry& entry
    )
    {
        float width = ImGui::GetWindowSize().x;

        if (width < 1.0f)
            width = 1.0f;

        ImGui::PushID(index);

        ImGui::SetCursorPosX(0.0f);

        ImGui::InvisibleButton(
            "TreeEntry",
            ImVec2(width, kTreeRowHeight)
        );

        bool hovered = ImGui::IsItemHovered();
        bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
        bool selected = IsEntrySelected(state, entry);

        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();

        ImDrawList* draw = ImGui::GetWindowDrawList();

        ImVec2 rowMin = ImVec2(min.x + 5.0f, min.y);
        ImVec2 rowMax = ImVec2(max.x - 5.0f, max.y);

        if (selected)
        {
            draw->AddRectFilled(
                rowMin,
                rowMax,
                SelectedFillColor(),
                4.0f
            );

            draw->AddRectFilled(
                ImVec2(rowMin.x, rowMin.y + 4.0f),
                ImVec2(rowMin.x + 2.0f, rowMax.y - 4.0f),
                AccentColor(),
                2.0f
            );
        }
        else if (hovered)
        {
            draw->AddRectFilled(
                rowMin,
                rowMax,
                HoverFillColor(),
                4.0f
            );
        }

        float baseX =
            min.x +
            kTreeLeftPadding +
            static_cast<float>(entry.depth) * kTreeIndent;

        float textY =
            min.y +
            (kTreeRowHeight - ImGui::GetTextLineHeight()) * 0.5f;

        if (entry.isDirectory)
        {
            bool expanded = IsDirectoryExpanded(state, entry.absolutePath);
            const char* arrow = expanded ? "v" : ">";

            draw->AddText(
                ImVec2(baseX + 2.0f, textY),
                hovered ? AccentColor() : MutedTextColor(),
                arrow
            );

            draw->AddText(
                ImVec2(baseX + 18.0f, textY),
                hovered ? BrightTextColor() : NormalTextColor(),
                entry.name.c_str()
            );

            if (clicked && intent)
                intent->toggleWorkspaceEntryIndex = index;
        }
        else
        {
            draw->AddText(
                ImVec2(baseX + 18.0f, textY),
                selected
                    ? BrightTextColor()
                    : hovered
                        ? BrightTextColor()
                        : NormalTextColor(),
                entry.name.c_str()
            );

            if (clicked && intent)
                intent->openWorkspaceEntryIndex = index;
        }

        if (hovered)
            ImGui::SetTooltip("%s", entry.displayPath.c_str());

        ImGui::PopID();
    }

    void DrawTreeArea(EditorState& state, EditorUiIntent* intent)
    {
        if (!state.hasWorkspace)
        {
            ImGui::SetCursorPos(
                ImVec2(
                    kHeaderLeftPadding,
                    kTreeTopPadding + 4.0f
                )
            );

            ImGui::TextColored(
                ImVec4(0.55f, 0.55f, 0.55f, 1.0f),
                "%s",
                GetPlaceholderText(ExplorerPage::Files)
            );

            DrawOpenFolderRow(intent);
            return;
        }

        ImGui::SetCursorPos(
            ImVec2(
                0.0f,
                kTreeTopPadding
            )
        );

        for (int i = 0; i < static_cast<int>(state.workspaceEntries.size()); ++i)
        {
            DrawTreeEntry(
                state,
                intent,
                i,
                state.workspaceEntries[i]
            );
        }

        if (state.workspaceEntries.empty())
        {
            ImGui::SetCursorPos(
                ImVec2(
                    kHeaderLeftPadding,
                    kTreeTopPadding + 6.0f
                )
            );

            ImGui::TextColored(
                ImVec4(0.55f, 0.55f, 0.55f, 1.0f),
                "This folder is empty."
            );
        }
    }
}

void DrawExplorer(
    EditorState& state,
    EditorUiIntent* intent,
    const char* id,
    float sidebarX,
    float sidebarWidth,
    float marginFromSideBar,
    float width,
    float marginTop,
    float marginBottom,
    float rounding,
    ImU32 color
)
{
    ImVec2 windowSize = ImGui::GetWindowSize();

    float x = sidebarX + sidebarWidth + marginFromSideBar;

    ImVec2 position = ImVec2(x, marginTop);
    ImVec2 size = ImVec2(width, windowSize.y - marginTop - marginBottom);

    if (size.x < 0.0f)
        size.x = 0.0f;

    if (size.y < 0.0f)
        size.y = 0.0f;

    ImGui::SetCursorPos(position);

    ImVec2 panelMin = ImGui::GetCursorScreenPos();
    ImVec2 panelMax = ImVec2(panelMin.x + size.x, panelMin.y + size.y);

    ImDrawList* draw = ImGui::GetWindowDrawList();

    draw->AddRectFilled(
        panelMin,
        panelMax,
        color,
        rounding,
        ImDrawFlags_RoundCornersAll
    );

    std::string headerTitle;
    const char* headerSubtitle = nullptr;

    if (state.activePage == ExplorerPage::Files)
    {
        if (state.hasWorkspace)
        {
            headerTitle = GetWorkspaceDisplayName(state);
        }
        else
        {
            headerTitle = "Files";
            headerSubtitle = "No workspace open";
        }
    }
    else
    {
        headerTitle = GetExplorerTitle(state.activePage);
    }

    DrawFixedExplorerHeader(
        panelMin,
        panelMax,
        headerTitle,
        headerSubtitle
    );

    ImVec2 treePosition = ImVec2(
        position.x,
        position.y + kHeaderHeight + 1.0f
    );

    ImVec2 treeSize = ImVec2(
        size.x,
        size.y - kHeaderHeight - 1.0f
    );

    if (treeSize.y < 0.0f)
        treeSize.y = 0.0f;

    ImGui::SetCursorPos(treePosition);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));

    ImGuiWindowFlags treeFlags =
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollbar;

    std::string treeId = std::string(id) + "Tree";

    if (ImGui::BeginChild(treeId.c_str(), treeSize, false, treeFlags))
    {
        if (state.activePage == ExplorerPage::Files)
        {
            DrawTreeArea(state, intent);
        }
        else
        {
            ImGui::SetCursorPos(
                ImVec2(
                    kHeaderLeftPadding,
                    kTreeTopPadding + 6.0f
                )
            );

            ImGui::TextColored(
                ImVec4(0.55f, 0.55f, 0.55f, 1.0f),
                "%s",
                GetPlaceholderText(state.activePage)
            );
        }
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}
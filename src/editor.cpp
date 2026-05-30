#include "editor.h"
#include "ui.h"

#include <algorithm>

namespace
{
    constexpr float kExplorerDefaultWidth = 220.0f;
    constexpr float kExplorerMinWidth = 0.0f;
    constexpr float kExplorerAutoCloseWidth = 1.0f;
    constexpr float kExplorerMaxWidth = 1000.0f;
    constexpr float kMinimumMainPanelWidth = 100.0f;
    constexpr float kExplorerResizeHitboxWidth = 10.0f;

    struct ResizeHandleState
    {
        bool hovered = false;
        bool active = false;
    };

    float ClampExplorerWidth(
        float width,
        const ImVec2& displaySize,
        float explorerX,
        float marginFromExplorer,
        float rightMargin
    )
    {
        float maxWidthFromWindow =
            displaySize.x -
            explorerX -
            marginFromExplorer -
            rightMargin -
            kMinimumMainPanelWidth;

        float dynamicMaxWidth = std::min(
            kExplorerMaxWidth,
            maxWidthFromWindow
        );

        dynamicMaxWidth = std::max(
            dynamicMaxWidth,
            kExplorerMinWidth
        );

        return std::clamp(
            width,
            kExplorerMinWidth,
            dynamicMaxWidth
        );
    }

    ResizeHandleState DrawExplorerResizeHandle(
        float explorerX,
        float& explorerWidth,
        float explorerTopMargin,
        float explorerBottomMargin,
        const ImVec2& displaySize,
        float marginFromExplorer,
        float rightMargin
    )
    {
        ResizeHandleState state;

        explorerWidth = ClampExplorerWidth(
            explorerWidth,
            displaySize,
            explorerX,
            marginFromExplorer,
            rightMargin
        );

        float handleHeight = displaySize.y - explorerTopMargin - explorerBottomMargin;

        if (handleHeight < 0.0f)
            handleHeight = 0.0f;

        float handleX =
            explorerX +
            explorerWidth -
            kExplorerResizeHitboxWidth * 0.5f;

        ImGui::SetCursorPos(
            ImVec2(
                handleX,
                explorerTopMargin
            )
        );

        ImGui::InvisibleButton(
            "ExplorerResizeHandle",
            ImVec2(
                kExplorerResizeHitboxWidth,
                handleHeight
            )
        );

        state.hovered = ImGui::IsItemHovered();
        state.active = ImGui::IsItemActive();

        if (state.hovered || state.active)
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

        if (state.active)
        {
            explorerWidth += ImGui::GetIO().MouseDelta.x;

            explorerWidth = ClampExplorerWidth(
                explorerWidth,
                displaySize,
                explorerX,
                marginFromExplorer,
                rightMargin
            );
        }

        float visualHandleX =
            explorerX +
            explorerWidth -
            kExplorerResizeHitboxWidth * 0.5f;

        ImGui::SetCursorPos(
            ImVec2(
                visualHandleX,
                explorerTopMargin
            )
        );

        ImVec2 min = ImGui::GetCursorScreenPos();

        ImVec2 max = ImVec2(
            min.x + kExplorerResizeHitboxWidth,
            min.y + handleHeight
        );

        ImDrawList* draw = ImGui::GetWindowDrawList();

        ImU32 hoverFillColor = state.active
            ? IM_COL32(167, 139, 250, 45)
            : state.hovered
                ? IM_COL32(167, 139, 250, 25)
                : IM_COL32(0, 0, 0, 0);

        if (state.hovered || state.active)
        {
            draw->AddRectFilled(
                min,
                max,
                hoverFillColor,
                3.0f
            );
        }

        float lineX = min.x + kExplorerResizeHitboxWidth * 0.5f;

        ImU32 lineColor = state.active
            ? IM_COL32(167, 139, 250, 255)
            : state.hovered
                ? IM_COL32(167, 139, 250, 190)
                : IM_COL32(90, 90, 90, 100);

        float lineThickness = state.active || state.hovered
            ? 2.0f
            : 1.0f;

        draw->AddLine(
            ImVec2(lineX, min.y + 8.0f),
            ImVec2(lineX, max.y - 8.0f),
            lineColor,
            lineThickness
        );

        return state;
    }
}

bool DrawEditor(
    EditorState& state,
    const ImVec2& displaySize,
    ImVec2* editorMin,
    ImVec2* editorMax,
    EditorUiIntent* intent
)
{
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(displaySize);

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleColor(
        ImGuiCol_WindowBg,
        IM_COL32(24, 24, 28, 255)
    );

    ImGui::Begin("ImEditor Main Window", nullptr, windowFlags);

    float sidebarX = 0.0f;
    float sidebarWidth = 60.0f;
    float sidebarTopMargin = 10.0f;
    float sidebarBottomMargin = 30.0f;
    float sidebarRounding = 8.0f;

    float bottomBarHeight = 20.0f;

    float explorerMarginFromSidebar = 10.0f;

    float explorerTopMargin = 10.0f;
    float explorerBottomMargin = 30.0f;
    float explorerRounding = 8.0f;

    float explorerX = sidebarX + sidebarWidth + explorerMarginFromSidebar;

    float tabBarMarginFromExplorer = 10.0f;
    float tabBarTopMargin = 10.0f;
    float tabBarHeight = 40.0f;
    float tabBarRightMargin = 10.0f;
    float tabBarRounding = 8.0f;

    float mainPanelMarginFromExplorer = 10.0f;
    float mainPanelMarginFromTabBar = 10.0f;
    float mainPanelMarginFromBottomBar = 10.0f;
    float mainPanelRightMargin = 10.0f;
    float mainPanelRounding = 8.0f;

    state.explorerWidth = ClampExplorerWidth(
        state.explorerWidth,
        displaySize,
        explorerX,
        tabBarMarginFromExplorer,
        tabBarRightMargin
    );

    DrawSideBar(
        state,
        intent,
        "SideBar",
        sidebarX,
        sidebarWidth,
        sidebarTopMargin,
        sidebarBottomMargin,
        sidebarRounding,
        IM_COL32(34, 34, 40, 255)
    );

    DrawBottomBar(
        state,
        bottomBarHeight,
        IM_COL32(34, 34, 40, 255)
    );

    if (state.explorerOpen)
    {
        DrawExplorer(
            state,
            intent,
            "Explorer",
            sidebarX,
            sidebarWidth,
            explorerMarginFromSidebar,
            state.explorerWidth,
            explorerTopMargin,
            explorerBottomMargin,
            explorerRounding,
            IM_COL32(34, 34, 40, 255)
        );

        DrawExplorerResizeHandle(
            explorerX,
            state.explorerWidth,
            explorerTopMargin,
            explorerBottomMargin,
            displaySize,
            tabBarMarginFromExplorer,
            tabBarRightMargin
        );

        if (state.explorerWidth <= kExplorerAutoCloseWidth)
        {
            state.explorerOpen = false;
            state.explorerWidth = kExplorerDefaultWidth;
        }
    }

    float layoutAnchorX = state.explorerOpen
        ? explorerX
        : sidebarX;

    float layoutAnchorWidth = state.explorerOpen
        ? state.explorerWidth
        : sidebarWidth;

    DrawTabBar(
        state,
        intent,
        layoutAnchorX,
        layoutAnchorWidth,
        tabBarMarginFromExplorer,
        tabBarTopMargin,
        tabBarHeight,
        tabBarRightMargin,
        tabBarRounding,
        IM_COL32(34, 34, 40, 255)
    );

    bool hasEditorRect = DrawCodeEditor(
        state,
        "CodeEditor",
        layoutAnchorX,
        layoutAnchorWidth,
        mainPanelMarginFromExplorer,
        tabBarTopMargin,
        tabBarHeight,
        mainPanelMarginFromTabBar,
        bottomBarHeight,
        mainPanelMarginFromBottomBar,
        mainPanelRightMargin,
        mainPanelRounding,
        IM_COL32(30, 29, 36, 255),
        editorMin,
        editorMax
    );

    ImGui::End();

    ImGui::PopStyleColor();

    return hasEditorRect;
}
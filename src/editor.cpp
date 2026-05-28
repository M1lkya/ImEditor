#include "editor.h"
#include "ui.h"

void DrawEditor(const ImVec2& Display_Size)
{
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(Display_Size);

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::Begin("ImEditor Main Window", nullptr, window_flags);

    float sidebarX = 0.0f;
    float sidebarWidth = 60.0f;
    float sidebarTopMargin = 10.0f;
    float sidebarBottomMargin = 30.0f;
    float sidebarRounding = 8.0f;

    float bottomBarHeight = 20.0f;

    float explorerMarginFromSidebar = 10.0f;
    float explorerWidth = 250.0f;
    float explorerTopMargin = 10.0f;
    float explorerBottomMargin = 30.0f;
    float explorerRounding = 8.0f;

    float explorerX = sidebarX + sidebarWidth + explorerMarginFromSidebar;

    float tabBarMarginFromExplorer = 10.0f;
    float tabBarTopMargin = 10.0f;
    float tabBarHeight = 50.0f;
    float tabBarRightMargin = 10.0f;
    float tabBarRounding = 8.0f;
   
    float mainPanelMarginFromExplorer = 10.0f;
    float mainPanelMarginFromTabBar = 10.0f;
    float mainPanelMarginFromBottomBar = 10.0f;
    float mainPanelRightMargin = 10.0f;
    float mainPanelRounding = 8.0f;

    DrawSideBar(
        "SideBar",
        sidebarX,
        sidebarWidth,
        sidebarTopMargin,
        sidebarBottomMargin,
        sidebarRounding,
        IM_COL32(45, 45, 55, 255)
    );

    DrawBottomBar(
        bottomBarHeight,
        IM_COL32(45, 45, 55, 255)
    );

    DrawExplorer(
        "Explorer",
        sidebarX,
        sidebarWidth,
        explorerMarginFromSidebar,
        explorerWidth,
        explorerTopMargin,
        explorerBottomMargin,
        explorerRounding,
        IM_COL32(45, 45, 55, 255)
    );

    DrawTabBar(
        explorerX,
        explorerWidth,
        tabBarMarginFromExplorer,
        tabBarTopMargin,
        tabBarHeight,
        tabBarRightMargin,
        tabBarRounding,
        IM_COL32(45, 45, 55, 255)
    );

    DrawCodeEditor(
        "CodeEditor",
        explorerX,
        explorerWidth,
        mainPanelMarginFromExplorer,
        tabBarTopMargin,
        tabBarHeight,
        mainPanelMarginFromTabBar,
        bottomBarHeight,
        mainPanelMarginFromBottomBar,
        mainPanelRightMargin,
        mainPanelRounding,
        IM_COL32(35, 35, 45, 255)
    );

    ImGui::End();
}
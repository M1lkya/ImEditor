#pragma once

#include "imgui.h"

enum class ExplorerPage
{
    Files,
    SourceControl,
    Extensions,
    Settings
};

struct SideBarState
{
    ExplorerPage activePage = ExplorerPage::Files;
    bool explorerOpen = true;
};

SideBarState DrawSideBar(
    const char* id,
    float x,
    float width,
    float marginTop,
    float marginBottom,
    float rounding,
    ImU32 color
);

void DrawBottomBar(
    float height,
    ImU32 color
);

void DrawExplorer(
    const char* id,
    ExplorerPage page,
    float sidebarX,
    float sidebarWidth,
    float marginFromSideBar,
    float width,
    float marginTop,
    float marginBottom,
    float rounding,
    ImU32 color
);

void DrawTabBar(
    float explorerX,
    float explorerWidth,
    float marginFromExplorer,
    float marginTop,
    float height,
    float marginRight,
    float rounding,
    ImU32 color
);

bool DrawCodeEditor(
    const char* id,
    float explorerX,
    float explorerWidth,
    float marginFromExplorer,
    float tabBarY,
    float tabBarHeight,
    float marginFromTabBar,
    float bottomBarHeight,
    float marginFromBottomBar,
    float marginRight,
    float rounding,
    ImU32 color,
    ImVec2* outMin,
    ImVec2* outMax,
    float customWidth = 0.0f,
    float customHeight = 0.0f
);
#pragma once

#include "imgui.h"
#include "EditorState.h"

void DrawSideBar(
    EditorState& state,
    EditorUiIntent* intent,
    const char* id,
    float x,
    float width,
    float marginTop,
    float marginBottom,
    float rounding,
    ImU32 color
);

void DrawBottomBar(
    const EditorState& state,
    float height,
    ImU32 color
);

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
);

void DrawTabBar(
    EditorState& state,
    EditorUiIntent* intent,
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
    const EditorState& state,
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
#pragma once

#include "imgui.h"

struct TabResult
{
    bool clicked = false;
    bool closeClicked = false;
    float width = 0.0f;
};

struct SideBarButtonResult
{
    bool clicked = false;
    bool hovered = false;
    bool active = false;
};

float CalculateTabWidth(const char* fileName);

TabResult DrawTab(
    const char* id,
    ImVec2 localPosition,
    const char* fileName,
    float tabHeight = 30.0f,
    ImTextureID iconTexture = 0
);

SideBarButtonResult DrawSideBarButton(
    const char* id,
    ImVec2 localPosition,
    const char* icon,
    bool active = false,
    float buttonSize = 42.0f
);
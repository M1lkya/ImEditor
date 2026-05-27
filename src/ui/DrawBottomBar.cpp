#include "ui.h"

void DrawBottomBar(
    float height,
    ImU32 color
)
{
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();

    ImVec2 max = ImVec2(
        windowPos.x + windowSize.x,
        windowPos.y + windowSize.y
    );

    ImVec2 min = ImVec2(
        windowPos.x,
        max.y - height
    );

    ImDrawList* draw = ImGui::GetWindowDrawList();

    draw->AddRectFilled(
        min,
        max,
        color
    );
}
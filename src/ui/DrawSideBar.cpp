#include "ui.h"

void DrawSideBar(
    const char* id,
    float x,
    float width,
    float marginTop,
    float marginBottom,
    float rounding,
    ImU32 color
)
{
    ImVec2 windowSize = ImGui::GetWindowSize();

    ImVec2 position = ImVec2(x, marginTop);
    ImVec2 size = ImVec2(width, windowSize.y - marginTop - marginBottom);

    ImGui::SetCursorPos(position);

    ImVec2 min = ImGui::GetCursorScreenPos();
    ImVec2 max = ImVec2(min.x + size.x, min.y + size.y);

    ImDrawList* draw = ImGui::GetWindowDrawList();

    draw->AddRectFilled(
        min,
        max,
        color,
        rounding,
        ImDrawFlags_RoundCornersRight
    );

    ImGui::Dummy(size);
}
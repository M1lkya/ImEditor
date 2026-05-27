#include "ui.h"

void DrawTabBar(
    float explorerX,
    float explorerWidth,
    float marginFromExplorer,
    float marginTop,
    float height,
    float marginRight,
    float rounding,
    ImU32 color
)
{
    ImVec2 windowSize = ImGui::GetWindowSize();

    float x = explorerX + explorerWidth + marginFromExplorer;
    float width = windowSize.x - x - marginRight;

    ImVec2 position = ImVec2(x, marginTop);
    ImVec2 size = ImVec2(width, height);

    ImGui::SetCursorPos(position);

    ImVec2 min = ImGui::GetCursorScreenPos();
    ImVec2 max = ImVec2(min.x + size.x, min.y + size.y);

    ImDrawList* draw = ImGui::GetWindowDrawList();

    draw->AddRectFilled(
        min,
        max,
        color,
        rounding,
        ImDrawFlags_RoundCornersAll
    );

    ImGui::Dummy(size);
}
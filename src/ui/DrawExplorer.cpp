#include "ui.h"

void DrawExplorer(
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
    (void)id;

    ImVec2 windowSize = ImGui::GetWindowSize();

    float x = sidebarX + sidebarWidth + marginFromSideBar;

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
        ImDrawFlags_RoundCornersAll
    );

    ImGui::Dummy(size);
}
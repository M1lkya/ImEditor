#include "components.h"

bool DrawTab(
    const char* id,
    ImVec2 localPosition,
    ImVec2 size
)
{
    ImGui::SetCursorPos(localPosition);

    ImGui::InvisibleButton(id, size);

    bool hovered = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked();

    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();

    ImDrawList* draw = ImGui::GetWindowDrawList();

    ImU32 tabColor = hovered
        ? IM_COL32(255, 170, 40, 255)
        : IM_COL32(255, 145, 0, 255);

    draw->AddRectFilled(
        min,
        max,
        tabColor,
        6.0f,
        ImDrawFlags_RoundCornersAll
    );

    return clicked;
}
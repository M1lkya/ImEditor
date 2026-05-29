#include "components.h"

#include <algorithm>

static constexpr bool kShowTabImages = false;

float CalculateTabWidth(const char* fileName)
{
    const float minWidth = 110.0f;

    const float leftPadding = 10.0f;
    const float iconSize = 15.0f;
    const float iconTextGap = 8.0f;
    const float textCloseGap = 12.0f;
    const float closeButtonSize = 16.0f;
    const float rightPadding = 8.0f;

    ImVec2 textSize = ImGui::CalcTextSize(fileName);

    float width =
        leftPadding +
        textSize.x +
        textCloseGap +
        closeButtonSize +
        rightPadding;

    if constexpr (kShowTabImages)
    {
        width += iconSize + iconTextGap;
    }

    return std::max(width, minWidth);
}

TabResult DrawTab(
    const char* id,
    ImVec2 localPosition,
    const char* fileName,
    float tabHeight,
    ImTextureID iconTexture
)
{
    TabResult result;

    const float tabWidth = CalculateTabWidth(fileName);
    result.width = tabWidth;

    ImVec2 size = ImVec2(tabWidth, tabHeight);

    ImGui::SetCursorPos(localPosition);
    ImGui::InvisibleButton(id, size);

    bool hovered = ImGui::IsItemHovered();

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

    const float centerY = min.y + tabHeight * 0.5f;

    const float leftPadding = 10.0f;
    const float iconSize = 15.0f;
    const float iconTextGap = 8.0f;
    const float closeButtonSize = 16.0f;

    float textAreaMinX = min.x + leftPadding;

    if constexpr (kShowTabImages)
    {
        ImVec2 iconMin = ImVec2(
            min.x + leftPadding,
            centerY - iconSize * 0.5f
        );

        ImVec2 iconMax = ImVec2(
            iconMin.x + iconSize,
            iconMin.y + iconSize
        );

        if (iconTexture != 0)
        {
            draw->AddImage(iconTexture, iconMin, iconMax);
        }
        else
        {
            draw->AddRectFilled(
                iconMin,
                iconMax,
                IM_COL32(255, 220, 120, 255),
                3.0f
            );
        }

        textAreaMinX = iconMax.x + iconTextGap;
    }

    ImVec2 closeMin = ImVec2(
        max.x - closeButtonSize - 8.0f,
        centerY - closeButtonSize * 0.5f
    );

    ImVec2 closeMax = ImVec2(
        closeMin.x + closeButtonSize,
        closeMin.y + closeButtonSize
    );

    bool closeHovered = ImGui::IsMouseHoveringRect(closeMin, closeMax);

    result.closeClicked =
        closeHovered &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    result.clicked =
        ImGui::IsItemClicked(ImGuiMouseButton_Left) &&
        !result.closeClicked;

    ImVec2 textSize = ImGui::CalcTextSize(fileName);

    float textAreaMaxX = closeMin.x - 8.0f;
    float textAreaWidth = textAreaMaxX - textAreaMinX;

    ImVec2 textPos = ImVec2(
        textAreaMinX + (textAreaWidth - textSize.x) * 0.5f,
        centerY - textSize.y * 0.5f
    );

    draw->AddText(
        textPos,
        IM_COL32(35, 35, 35, 255),
        fileName
    );

    ImU32 closeColor = closeHovered
        ? IM_COL32(80, 80, 80, 220)
        : IM_COL32(50, 50, 50, 150);

    draw->AddRectFilled(
        closeMin,
        closeMax,
        closeColor,
        4.0f
    );

    const float xPad = 4.5f;

    draw->AddLine(
        ImVec2(closeMin.x + xPad, closeMin.y + xPad),
        ImVec2(closeMax.x - xPad, closeMax.y - xPad),
        IM_COL32(240, 240, 240, 255),
        1.4f
    );

    draw->AddLine(
        ImVec2(closeMax.x - xPad, closeMin.y + xPad),
        ImVec2(closeMin.x + xPad, closeMax.y - xPad),
        IM_COL32(240, 240, 240, 255),
        1.4f
    );

    return result;
}
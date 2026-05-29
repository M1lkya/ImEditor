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
    const float closeButtonSize = 18.0f;
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

    const ImU32 tabColor = hovered
        ? IM_COL32(52, 52, 52, 255)
        : IM_COL32(45, 45, 45, 255);

    const ImU32 tabBorderColor = hovered
        ? IM_COL32(78, 78, 78, 220)
        : IM_COL32(60, 60, 60, 160);

    draw->AddRectFilled(
        min,
        max,
        tabColor,
        6.0f,
        ImDrawFlags_RoundCornersAll
    );

    draw->AddRect(
        min,
        max,
        tabBorderColor,
        6.0f,
        ImDrawFlags_RoundCornersAll,
        1.0f
    );

    draw->AddLine(
        ImVec2(min.x + 6.0f, min.y + 1.0f),
        ImVec2(max.x - 6.0f, min.y + 1.0f),
        IM_COL32(90, 90, 90, hovered ? 160 : 90),
        1.0f
    );

    const float centerY = min.y + tabHeight * 0.5f;

    const float leftPadding = 10.0f;
    const float iconSize = 15.0f;
    const float iconTextGap = 8.0f;
    const float closeButtonSize = 18.0f;

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
                IM_COL32(95, 95, 95, 255),
                3.0f
            );
        }

        textAreaMinX = iconMax.x + iconTextGap;
    }

    ImVec2 closeMin = ImVec2(
        max.x - closeButtonSize - 7.0f,
        centerY - closeButtonSize * 0.5f
    );

    ImVec2 closeMax = ImVec2(
        closeMin.x + closeButtonSize,
        closeMin.y + closeButtonSize
    );

    bool closeHovered =
        hovered &&
        ImGui::IsMouseHoveringRect(closeMin, closeMax);

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

    draw->PushClipRect(
        ImVec2(textAreaMinX, min.y),
        ImVec2(textAreaMaxX, max.y),
        true
    );

    draw->AddText(
        textPos,
        hovered
            ? IM_COL32(245, 245, 245, 255)
            : IM_COL32(215, 215, 215, 255),
        fileName
    );

    draw->PopClipRect();

    if (closeHovered)
    {
        draw->AddRectFilled(
            closeMin,
            closeMax,
            IM_COL32(78, 78, 78, 230),
            5.0f,
            ImDrawFlags_RoundCornersAll
        );

        draw->AddRect(
            closeMin,
            closeMax,
            IM_COL32(105, 105, 105, 180),
            5.0f,
            ImDrawFlags_RoundCornersAll,
            1.0f
        );
    }

    const float xPad = 4.75f;

    const ImU32 xColor = closeHovered
        ? IM_COL32(255, 255, 255, 255)
        : IM_COL32(165, 165, 165, 230);

    draw->AddLine(
        ImVec2(closeMin.x + xPad, closeMin.y + xPad),
        ImVec2(closeMax.x - xPad, closeMax.y - xPad),
        xColor,
        closeHovered ? 1.7f : 1.5f
    );

    draw->AddLine(
        ImVec2(closeMax.x - xPad, closeMin.y + xPad),
        ImVec2(closeMin.x + xPad, closeMax.y - xPad),
        xColor,
        closeHovered ? 1.7f : 1.5f
    );

    return result;
}
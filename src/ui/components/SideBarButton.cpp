#include "components.h"

#include <cfloat>

namespace
{
    ImWchar DecodeFirstUtf8Codepoint(const char* text)
    {
        if (!text || !text[0])
            return 0;

        const unsigned char* bytes =
            reinterpret_cast<const unsigned char*>(text);

        if (bytes[0] < 0x80)
            return static_cast<ImWchar>(bytes[0]);

        if ((bytes[0] & 0xE0) == 0xC0)
        {
            return static_cast<ImWchar>(
                ((bytes[0] & 0x1F) << 6) |
                (bytes[1] & 0x3F)
            );
        }

        if ((bytes[0] & 0xF0) == 0xE0)
        {
            return static_cast<ImWchar>(
                ((bytes[0] & 0x0F) << 12) |
                ((bytes[1] & 0x3F) << 6) |
                (bytes[2] & 0x3F)
            );
        }

        return 0;
    }

    ImVec2 CalculateCenteredIconPosition(
        ImFont* font,
        const char* icon,
        float iconFontSize,
        const ImVec2& boxMin,
        const ImVec2& boxMax
    )
    {
        ImVec2 boxCenter = ImVec2(
            (boxMin.x + boxMax.x) * 0.5f,
            (boxMin.y + boxMax.y) * 0.5f
        );

        if (!font)
            return boxCenter;

        ImWchar codepoint = DecodeFirstUtf8Codepoint(icon);

        ImFontBaked* bakedFont = font->GetFontBaked(iconFontSize);

        const ImFontGlyph* glyph =
            bakedFont && codepoint != 0
                ? bakedFont->FindGlyphNoFallback(codepoint)
                : nullptr;

        if (!glyph)
        {
            ImVec2 fallbackSize = font->CalcTextSizeA(
                iconFontSize,
                FLT_MAX,
                0.0f,
                icon
            );

            return ImVec2(
                boxCenter.x - fallbackSize.x * 0.5f,
                boxCenter.y - fallbackSize.y * 0.5f
            );
        }

        const float glyphWidth = glyph->X1 - glyph->X0;
        const float glyphHeight = glyph->Y1 - glyph->Y0;

        return ImVec2(
            boxCenter.x - glyphWidth * 0.5f - glyph->X0,
            boxCenter.y - glyphHeight * 0.5f - glyph->Y0
        );
    }
}

SideBarButtonResult DrawSideBarButton(
    const char* id,
    ImVec2 localPosition,
    const char* icon,
    bool active,
    float buttonSize
)
{
    SideBarButtonResult result;
    result.active = active;

    ImGui::SetCursorPos(localPosition);
    ImGui::InvisibleButton(id, ImVec2(buttonSize, buttonSize));

    result.hovered = ImGui::IsItemHovered();
    result.clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);

    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();

    ImDrawList* draw = ImGui::GetWindowDrawList();

    const ImU32 iconColor = active
        ? IM_COL32(255, 255, 255, 255)
        : result.hovered
            ? IM_COL32(225, 225, 225, 255)
            : IM_COL32(165, 165, 165, 255);

    if (active)
    {
        draw->AddRectFilled(
            ImVec2(min.x - 6.0f, min.y),
            ImVec2(min.x - 3.0f, max.y),
            IM_COL32(167, 139, 250, 255),
            2.0f,
            ImDrawFlags_RoundCornersRight
        );
    }

    ImFont* font = ImGui::GetFont();

    const float iconFontSize = 25.0f;

    ImVec2 iconPos = CalculateCenteredIconPosition(
        font,
        icon,
        iconFontSize,
        min,
        max
    );

    draw->AddText(
        font,
        iconFontSize,
        iconPos,
        iconColor,
        icon
    );

    return result;
}
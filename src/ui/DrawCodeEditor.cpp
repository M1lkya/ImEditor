#include "ui.h"

bool DrawCodeEditor(
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
    float customWidth,
    float customHeight
)
{
    (void)id;

    ImVec2 windowSize = ImGui::GetWindowSize();

    float x = explorerX + explorerWidth + marginFromExplorer;
    float y = tabBarY + tabBarHeight + marginFromTabBar;

    float width = customWidth > 0.0f
        ? customWidth
        : windowSize.x - x - marginRight;

    float height = customHeight > 0.0f
        ? customHeight
        : windowSize.y - y - bottomBarHeight - marginFromBottomBar;

    if (width < 0.0f)
        width = 0.0f;

    if (height < 0.0f)
        height = 0.0f;

    ImVec2 position = ImVec2(x, y);
    ImVec2 size = ImVec2(width, height);

    ImGui::SetCursorPos(position);

    ImVec2 min = ImGui::GetCursorScreenPos();
    ImVec2 max = ImVec2(min.x + size.x, min.y + size.y);

    if (outMin)
        *outMin = min;

    if (outMax)
        *outMax = max;

    ImDrawList* draw = ImGui::GetWindowDrawList();

    draw->AddRectFilled(
        min,
        max,
        color,
        rounding,
        ImDrawFlags_RoundCornersAll
    );

    ImGui::Dummy(size);

    return width > 0.0f && height > 0.0f;
}
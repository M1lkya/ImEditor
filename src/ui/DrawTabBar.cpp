#include "ui.h"
#include "components.h"

#include <algorithm>
#include <cstdio>

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
    static float scrollX = 0.0f;

    ImVec2 windowSize = ImGui::GetWindowSize();

    float x = explorerX + explorerWidth + marginFromExplorer;
    float width = windowSize.x - x - marginRight;

    if (width < 0.0f)
        width = 0.0f;

    ImVec2 position = ImVec2(x, marginTop);
    ImVec2 size = ImVec2(width, height);

    ImGui::SetCursorPos(position);

    ImGuiWindowFlags childFlags =
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));

    if (ImGui::BeginChild("TabBarChild", size, false, childFlags))
    {
        ImVec2 childMin = ImGui::GetWindowPos();
        ImVec2 childSize = ImGui::GetWindowSize();
        ImVec2 childMax = ImVec2(
            childMin.x + childSize.x,
            childMin.y + childSize.y
        );

        ImDrawList* draw = ImGui::GetWindowDrawList();

        draw->AddRectFilled(
            childMin,
            childMax,
            color,
            rounding,
            ImDrawFlags_RoundCornersAll
        );

        const float tabWidth = 120.0f;
        const float tabHeight = 40.0f;
        const float tabSpacing = 4.0f;
        const float tabStartX = 4.0f;
        const float tabY = 5.0f;
        const int tabCount = 5;

        float totalTabsWidth =
            tabStartX +
            tabCount * tabWidth +
            (tabCount - 1) * tabSpacing +
            4.0f;

        float maxScrollX = std::max(0.0f, totalTabsWidth - childSize.x);

        scrollX = std::clamp(scrollX, 0.0f, maxScrollX);

        draw->PushClipRect(childMin, childMax, true);

        for (int i = 0; i < tabCount; ++i)
        {
            char id[32];
            sprintf_s(id, sizeof(id), "Tab%d", i + 1);

            float tabX =
                tabStartX +
                i * (tabWidth + tabSpacing) -
                scrollX;

            if (tabX + tabWidth < 0.0f)
                continue;

            if (tabX > childSize.x)
                continue;

            DrawTab(
                id,
                ImVec2(tabX, tabY),
                ImVec2(tabWidth, tabHeight)
            );
        }

        draw->PopClipRect();

        if (maxScrollX > 0.0f)
        {
            const float trackPadding = 10.0f;
            const float trackHeight = 3.0f;
            const float trackY = childSize.y - 6.0f;

            float trackWidth = childSize.x - trackPadding * 2.0f;

            if (trackWidth > 0.0f)
            {
                float handleWidth =
                    trackWidth * (childSize.x / totalTabsWidth);

                handleWidth = std::clamp(handleWidth, 28.0f, trackWidth);

                float handleTravel = trackWidth - handleWidth;

                float handleX = trackPadding;

                if (maxScrollX > 0.0f && handleTravel > 0.0f)
                {
                    handleX += (scrollX / maxScrollX) * handleTravel;
                }

                ImVec2 trackMin = ImVec2(
                    childMin.x + trackPadding,
                    childMin.y + trackY
                );

                ImVec2 trackMax = ImVec2(
                    trackMin.x + trackWidth,
                    trackMin.y + trackHeight
                );

                ImVec2 handleMin = ImVec2(
                    childMin.x + handleX,
                    childMin.y + trackY
                );

                ImVec2 handleMax = ImVec2(
                    handleMin.x + handleWidth,
                    handleMin.y + trackHeight
                );

                draw->AddRectFilled(
                    trackMin,
                    trackMax,
                    IM_COL32(70, 70, 70, 140),
                    2.0f
                );

                draw->AddRectFilled(
                    handleMin,
                    handleMax,
                    IM_COL32(150, 150, 150, 210),
                    2.0f
                );

                ImGui::SetCursorScreenPos(ImVec2(trackMin.x, trackMin.y - 5.0f));
                ImGui::InvisibleButton(
                    "TabBarCustomScrollbar",
                    ImVec2(trackWidth, trackHeight + 10.0f)
                );

                if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                {
                    float mouseX = ImGui::GetIO().MousePos.x;
                    float localMouseX = mouseX - trackMin.x - handleWidth * 0.5f;

                    float t = 0.0f;

                    if (handleTravel > 0.0f)
                    {
                        t = localMouseX / handleTravel;
                    }

                    t = std::clamp(t, 0.0f, 1.0f);
                    scrollX = t * maxScrollX;
                }

                if (ImGui::IsWindowHovered())
                {
                    float wheel = ImGui::GetIO().MouseWheel;

                    if (wheel != 0.0f)
                    {
                        scrollX -= wheel * 40.0f;
                        scrollX = std::clamp(scrollX, 0.0f, maxScrollX);
                    }
                }
            }
        }
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}
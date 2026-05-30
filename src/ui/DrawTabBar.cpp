#include "ui.h"
#include "components.h"

#include <algorithm>
#include <cstdio>
#include <string>

void DrawTabBar(
    EditorState& state,
    EditorUiIntent* intent,
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

        float tabsVisibleWidth = childSize.x;

        if (tabsVisibleWidth < 0.0f)
            tabsVisibleWidth = 0.0f;

        const float tabHeight = 30.0f;
        const float tabSpacing = 4.0f;
        const float tabStartX = 4.0f;
        const float tabY = (childSize.y - tabHeight) * 0.5f;

        float totalTabsWidth = tabStartX + 4.0f;

        for (const EditorOpenFile& file : state.openFiles)
        {
            std::string label = file.title;

            if (file.dirty)
                label += " *";

            totalTabsWidth += CalculateTabWidth(label.c_str());
            totalTabsWidth += tabSpacing;
        }

        float maxScrollX = std::max(0.0f, totalTabsWidth - tabsVisibleWidth);
        state.tabScrollX = std::clamp(state.tabScrollX, 0.0f, maxScrollX);

        ImVec2 clipMin = childMin;
        ImVec2 clipMax = ImVec2(childMin.x + tabsVisibleWidth, childMax.y);

        draw->PushClipRect(clipMin, clipMax, true);

        float currentX = tabStartX;

        for (int i = 0; i < static_cast<int>(state.openFiles.size()); ++i)
        {
            const EditorOpenFile& file = state.openFiles[i];

            std::string label = file.title;

            if (file.dirty)
                label += " *";

            char id[64];
            sprintf_s(id, sizeof(id), "Tab%d", i + 1);

            float tabWidth = CalculateTabWidth(label.c_str());
            float tabX = currentX - state.tabScrollX;

            if (tabX + tabWidth >= 0.0f && tabX <= tabsVisibleWidth)
            {
                TabResult tab = DrawTab(
                    id,
                    ImVec2(tabX, tabY),
                    label.c_str(),
                    tabHeight
                );

                if (i == state.activeFileIndex)
                {
                    ImVec2 tabMin = ImVec2(childMin.x + tabX, childMin.y + tabY);
                    ImVec2 tabMax = ImVec2(tabMin.x + tabWidth, tabMin.y + tabHeight);

                    draw->AddRect(
                        tabMin,
                        tabMax,
                        IM_COL32(167, 139, 250, 220),
                        6.0f,
                        ImDrawFlags_RoundCornersAll,
                        1.5f
                    );
                }

                if (tab.closeClicked)
                {
                    if (intent)
                        intent->closeTabIndex = i;
                }
                else if (tab.clicked)
                {
                    if (intent)
                        intent->selectTabIndex = i;
                }

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("%s", file.displayPath.c_str());
            }

            currentX += tabWidth + tabSpacing;
        }

        draw->PopClipRect();

        if (maxScrollX > 0.0f && tabsVisibleWidth > 0.0f)
        {
            const float trackPadding = 10.0f;
            const float trackHeight = 3.0f;
            const float trackY = childSize.y - 5.0f;

            float trackWidth = tabsVisibleWidth - trackPadding * 2.0f;

            if (trackWidth > 0.0f)
            {
                float handleWidth =
                    trackWidth * (tabsVisibleWidth / totalTabsWidth);

                handleWidth = std::clamp(handleWidth, 28.0f, trackWidth);

                float handleTravel = trackWidth - handleWidth;
                float handleX = trackPadding;

                if (maxScrollX > 0.0f && handleTravel > 0.0f)
                    handleX += (state.tabScrollX / maxScrollX) * handleTravel;

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
                    float localMouseX =
                        mouseX - trackMin.x - handleWidth * 0.5f;

                    float t = 0.0f;

                    if (handleTravel > 0.0f)
                        t = localMouseX / handleTravel;

                    t = std::clamp(t, 0.0f, 1.0f);
                    state.tabScrollX = t * maxScrollX;
                }

                if (ImGui::IsWindowHovered())
                {
                    float wheel = ImGui::GetIO().MouseWheel;

                    if (wheel != 0.0f)
                    {
                        state.tabScrollX -= wheel * 40.0f;
                        state.tabScrollX = std::clamp(state.tabScrollX, 0.0f, maxScrollX);
                    }
                }
            }
        }

        if (state.openFiles.empty())
        {
            draw->AddText(
                ImVec2(childMin.x + 12.0f, childMin.y + 12.0f),
                IM_COL32(150, 150, 150, 255),
                "No open files"
            );
        }
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}
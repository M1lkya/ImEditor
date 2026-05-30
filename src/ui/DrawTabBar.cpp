#include "ui.h"
#include "components.h"

#include <algorithm>
#include <cstdio>
#include <string>

namespace
{
    struct PageTabResult
    {
        bool clicked = false;
        bool closeClicked = false;
        float width = 0.0f;
    };

    float CalculatePageTabWidth(const char* label, bool closeable)
    {
        const float minWidth = 110.0f;
        ImVec2 textSize = ImGui::CalcTextSize(label);

        float width = textSize.x + 32.0f;

        if (closeable)
            width += 22.0f;

        return std::max(minWidth, width);
    }

    PageTabResult DrawPageTab(
        const char* id,
        ImVec2 localPosition,
        const char* label,
        float tabHeight,
        bool active,
        bool closeable
    )
    {
        PageTabResult result;
        result.width = CalculatePageTabWidth(label, closeable);

        ImVec2 size = ImVec2(result.width, tabHeight);

        ImGui::SetCursorPos(localPosition);
        ImGui::InvisibleButton(id, size);

        bool hovered = ImGui::IsItemHovered();

        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();

        ImDrawList* draw = ImGui::GetWindowDrawList();

        ImU32 fill = active
            ? IM_COL32(58, 54, 72, 255)
            : hovered
                ? IM_COL32(52, 52, 52, 255)
                : IM_COL32(45, 45, 45, 255);

        ImU32 border = active
            ? IM_COL32(167, 139, 250, 220)
            : hovered
                ? IM_COL32(78, 78, 78, 220)
                : IM_COL32(60, 60, 60, 160);

        draw->AddRectFilled(
            min,
            max,
            fill,
            6.0f,
            ImDrawFlags_RoundCornersAll
        );

        draw->AddRect(
            min,
            max,
            border,
            6.0f,
            ImDrawFlags_RoundCornersAll,
            active ? 1.5f : 1.0f
        );

        float closeButtonSize = 18.0f;
        float closePaddingRight = 7.0f;
        float textAreaMaxX = max.x - 10.0f;

        ImVec2 closeMin;
        ImVec2 closeMax;

        if (closeable)
        {
            closeMin = ImVec2(
                max.x - closeButtonSize - closePaddingRight,
                min.y + (tabHeight - closeButtonSize) * 0.5f
            );

            closeMax = ImVec2(
                closeMin.x + closeButtonSize,
                closeMin.y + closeButtonSize
            );

            textAreaMaxX = closeMin.x - 6.0f;
        }

        bool closeHovered =
            closeable &&
            hovered &&
            ImGui::IsMouseHoveringRect(closeMin, closeMax);

        result.closeClicked =
            closeHovered &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Left);

        result.clicked =
            ImGui::IsItemClicked(ImGuiMouseButton_Left) &&
            !result.closeClicked;

        ImVec2 textSize = ImGui::CalcTextSize(label);

        float textAreaMinX = min.x + 12.0f;
        float textAreaWidth = textAreaMaxX - textAreaMinX;

        draw->AddText(
            ImVec2(
                textAreaMinX + (textAreaWidth - textSize.x) * 0.5f,
                min.y + (tabHeight - textSize.y) * 0.5f
            ),
            active || hovered
                ? IM_COL32(245, 245, 245, 255)
                : IM_COL32(215, 215, 215, 255),
            label
        );

        if (closeable)
        {
            if (closeHovered)
            {
                draw->AddRectFilled(
                    closeMin,
                    closeMax,
                    IM_COL32(78, 78, 78, 230),
                    5.0f,
                    ImDrawFlags_RoundCornersAll
                );
            }

            const float xPad = 4.75f;

            ImU32 xColor = closeHovered
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
        }

        return result;
    }
}

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

        if (state.welcomeTabOpen)
        {
            totalTabsWidth += CalculatePageTabWidth("Welcome", true);
            totalTabsWidth += tabSpacing;
        }

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

        if (state.welcomeTabOpen)
        {
            float tabWidth = CalculatePageTabWidth("Welcome", true);
            float tabX = currentX - state.tabScrollX;

            if (tabX + tabWidth >= 0.0f && tabX <= tabsVisibleWidth)
            {
                PageTabResult tab = DrawPageTab(
                    "WelcomeTab",
                    ImVec2(tabX, tabY),
                    "Welcome",
                    tabHeight,
                    state.activeContentPage == ActiveContentPage::Welcome,
                    true
                );

                if (tab.closeClicked)
                {
                    if (intent)
                        intent->closeWelcomeTabRequested = true;
                }
                else if (tab.clicked)
                {
                    if (intent)
                        intent->selectWelcomeTabRequested = true;
                }
            }

            currentX += tabWidth + tabSpacing;
        }

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

                if (
                    state.activeContentPage == ActiveContentPage::Editor &&
                    i == state.activeFileIndex
                )
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
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}
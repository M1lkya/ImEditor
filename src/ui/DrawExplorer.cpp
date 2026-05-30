#include "ui.h"

namespace
{
    const char* GetExplorerTitle(ExplorerPage page)
    {
        switch (page)
        {
        case ExplorerPage::Files:
            return "Files";

        case ExplorerPage::SourceControl:
            return "Source Control";

        case ExplorerPage::Extensions:
            return "Extensions";

        case ExplorerPage::Settings:
            return "Settings";
        }

        return "Explorer";
    }

    const char* GetExplorerBodyText(ExplorerPage page)
    {
        switch (page)
        {
        case ExplorerPage::Files:
            return "Project files will go here.";

        case ExplorerPage::SourceControl:
            return "Source control changes will go here.";

        case ExplorerPage::Extensions:
            return "Extensions will go here.";

        case ExplorerPage::Settings:
            return "";
        }

        return "";
    }
}

void DrawExplorer(
    const char* id,
    ExplorerPage page,
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
    ImVec2 windowSize = ImGui::GetWindowSize();

    float x = sidebarX + sidebarWidth + marginFromSideBar;

    ImVec2 position = ImVec2(x, marginTop);
    ImVec2 size = ImVec2(width, windowSize.y - marginTop - marginBottom);

    if (size.x < 0.0f)
        size.x = 0.0f;

    if (size.y < 0.0f)
        size.y = 0.0f;

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

    ImGui::SetCursorPos(position);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 12.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));

    if (ImGui::BeginChild(id, size, false, ImGuiWindowFlags_NoScrollbar))
    {
        ImGui::TextUnformatted(GetExplorerTitle(page));

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (size.x > 90.0f)
        {
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + size.x - 20.0f);
            ImGui::TextUnformatted(GetExplorerBodyText(page));
            ImGui::PopTextWrapPos();
        }
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}
#include "ui.h"
#include "components.h"

namespace
{
    // UTF-8 bytes for Codicon private-use Unicode characters.
    constexpr const char* CODICON_FILES = "\xEE\xAB\xB0";          // U+EAF0
    constexpr const char* CODICON_SOURCE_CONTROL = "\xEE\xA9\xA8"; // U+EA68
    constexpr const char* CODICON_EXTENSIONS = "\xEE\xAB\xA6";     // U+EAE6
    constexpr const char* CODICON_SETTINGS = "\xEE\xAD\x91";       // U+EB51

    enum class SideBarItem
    {
        Files,
        SourceControl,
        Extensions,
        Settings
    };

    SideBarItem g_activeSideBarItem = SideBarItem::Files;
}

void DrawSideBar(
    const char* id,
    float x,
    float width,
    float marginTop,
    float marginBottom,
    float rounding,
    ImU32 color
)
{
    (void)id;

    ImVec2 windowSize = ImGui::GetWindowSize();

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
        ImDrawFlags_RoundCornersRight
    );

    const float buttonSize = 42.0f;
    const float buttonX = x + (width - buttonSize) * 0.5f;
    const float buttonGap = 8.0f;

    float currentY = marginTop + 12.0f;

    SideBarButtonResult filesButton = DrawSideBarButton(
        "SideBarFilesButton",
        ImVec2(buttonX, currentY),
        CODICON_FILES,
        g_activeSideBarItem == SideBarItem::Files,
        buttonSize
    );

    if (filesButton.clicked)
        g_activeSideBarItem = SideBarItem::Files;

    currentY += buttonSize + buttonGap;

    SideBarButtonResult sourceControlButton = DrawSideBarButton(
        "SideBarSourceControlButton",
        ImVec2(buttonX, currentY),
        CODICON_SOURCE_CONTROL,
        g_activeSideBarItem == SideBarItem::SourceControl,
        buttonSize
    );

    if (sourceControlButton.clicked)
        g_activeSideBarItem = SideBarItem::SourceControl;

    currentY += buttonSize + buttonGap;

    SideBarButtonResult extensionsButton = DrawSideBarButton(
        "SideBarExtensionsButton",
        ImVec2(buttonX, currentY),
        CODICON_EXTENSIONS,
        g_activeSideBarItem == SideBarItem::Extensions,
        buttonSize
    );

    if (extensionsButton.clicked)
        g_activeSideBarItem = SideBarItem::Extensions;

    const float settingsY = marginTop + size.y - buttonSize - 12.0f;

    SideBarButtonResult settingsButton = DrawSideBarButton(
        "SideBarSettingsButton",
        ImVec2(buttonX, settingsY),
        CODICON_SETTINGS,
        g_activeSideBarItem == SideBarItem::Settings,
        buttonSize
    );

    if (settingsButton.clicked)
        g_activeSideBarItem = SideBarItem::Settings;

    ImGui::SetCursorPos(position);
    ImGui::Dummy(size);
}
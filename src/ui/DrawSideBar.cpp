#include "ui.h"
#include "components.h"

namespace
{
    // UTF-8 bytes for Codicon private-use Unicode characters.
    constexpr const char* CODICON_FILES = "\xEE\xAB\xB0";          // U+EAF0
    constexpr const char* CODICON_SOURCE_CONTROL = "\xEE\xA9\xA8"; // U+EA68
    constexpr const char* CODICON_EXTENSIONS = "\xEE\xAB\xA6";     // U+EAE6
    constexpr const char* CODICON_SETTINGS = "\xEE\xAD\x91";       // U+EB51

    ExplorerPage g_activePage = ExplorerPage::Files;
    bool g_explorerOpen = true;

    void HandleSideBarPageClick(ExplorerPage page)
    {
        if (page == ExplorerPage::Settings)
        {
            g_activePage = ExplorerPage::Settings;
            g_explorerOpen = false;
            return;
        }

        if (g_activePage == page)
        {
            g_explorerOpen = !g_explorerOpen;
            return;
        }

        g_activePage = page;
        g_explorerOpen = true;
    }
}

SideBarState DrawSideBar(
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
        g_activePage == ExplorerPage::Files,
        buttonSize
    );

    if (filesButton.clicked)
        HandleSideBarPageClick(ExplorerPage::Files);

    currentY += buttonSize + buttonGap;

    SideBarButtonResult sourceControlButton = DrawSideBarButton(
        "SideBarSourceControlButton",
        ImVec2(buttonX, currentY),
        CODICON_SOURCE_CONTROL,
        g_activePage == ExplorerPage::SourceControl,
        buttonSize
    );

    if (sourceControlButton.clicked)
        HandleSideBarPageClick(ExplorerPage::SourceControl);

    currentY += buttonSize + buttonGap;

    SideBarButtonResult extensionsButton = DrawSideBarButton(
        "SideBarExtensionsButton",
        ImVec2(buttonX, currentY),
        CODICON_EXTENSIONS,
        g_activePage == ExplorerPage::Extensions,
        buttonSize
    );

    if (extensionsButton.clicked)
        HandleSideBarPageClick(ExplorerPage::Extensions);

    const float settingsY = marginTop + size.y - buttonSize - 12.0f;

    SideBarButtonResult settingsButton = DrawSideBarButton(
        "SideBarSettingsButton",
        ImVec2(buttonX, settingsY),
        CODICON_SETTINGS,
        g_activePage == ExplorerPage::Settings,
        buttonSize
    );

    if (settingsButton.clicked)
        HandleSideBarPageClick(ExplorerPage::Settings);

    ImGui::SetCursorPos(position);
    ImGui::Dummy(size);

    SideBarState state;
    state.activePage = g_activePage;
    state.explorerOpen = g_explorerOpen;

    return state;
}
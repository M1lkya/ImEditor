#include "ui.h"
#include "components.h"

namespace
{
    constexpr const char* CODICON_FILES = "\xEE\xAB\xB0";
    constexpr const char* CODICON_SOURCE_CONTROL = "\xEE\xA9\xA8";
    constexpr const char* CODICON_EXTENSIONS = "\xEE\xAB\xA6";
    constexpr const char* CODICON_SETTINGS = "\xEE\xAD\x91";

    constexpr float kExplorerDefaultWidth = 220.0f;

    void EnsureExplorerHasUsableWidth(EditorState& state)
    {
        if (state.explorerWidth <= 0.0f)
            state.explorerWidth = kExplorerDefaultWidth;
    }

    void HandleSideBarPageClick(EditorState& state, ExplorerPage page)
    {
        if (page == ExplorerPage::Settings)
        {
            state.activePage = ExplorerPage::Settings;
            state.explorerOpen = false;

            state.activeContentPage = ActiveContentPage::Settings;
            state.activeFileNeedsWebSync = true;
            return;
        }

        if (state.activePage == page)
        {
            state.explorerOpen = !state.explorerOpen;
        }
        else
        {
            state.activePage = page;
            state.explorerOpen = true;
        }

        if (state.explorerOpen)
            EnsureExplorerHasUsableWidth(state);

        if (HasActiveFile(state))
        {
            state.activeContentPage = ActiveContentPage::Editor;
        }
        else if (state.welcomeTabOpen)
        {
            state.activeContentPage = ActiveContentPage::Welcome;
        }
        else
        {
            state.activeContentPage = ActiveContentPage::None;
        }

        state.activeFileNeedsWebSync = true;
    }
}

void DrawSideBar(
    EditorState& state,
    EditorUiIntent* intent,
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
    (void)intent;

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
        state.activePage == ExplorerPage::Files,
        buttonSize
    );

    if (filesButton.clicked)
        HandleSideBarPageClick(state, ExplorerPage::Files);

    currentY += buttonSize + buttonGap;

    SideBarButtonResult sourceControlButton = DrawSideBarButton(
        "SideBarSourceControlButton",
        ImVec2(buttonX, currentY),
        CODICON_SOURCE_CONTROL,
        state.activePage == ExplorerPage::SourceControl,
        buttonSize
    );

    if (sourceControlButton.clicked)
        HandleSideBarPageClick(state, ExplorerPage::SourceControl);

    currentY += buttonSize + buttonGap;

    SideBarButtonResult extensionsButton = DrawSideBarButton(
        "SideBarExtensionsButton",
        ImVec2(buttonX, currentY),
        CODICON_EXTENSIONS,
        state.activePage == ExplorerPage::Extensions,
        buttonSize
    );

    if (extensionsButton.clicked)
        HandleSideBarPageClick(state, ExplorerPage::Extensions);

    const float settingsY = marginTop + size.y - buttonSize - 12.0f;

    SideBarButtonResult settingsButton = DrawSideBarButton(
        "SideBarSettingsButton",
        ImVec2(buttonX, settingsY),
        CODICON_SETTINGS,
        state.activePage == ExplorerPage::Settings,
        buttonSize
    );

    if (settingsButton.clicked)
        HandleSideBarPageClick(state, ExplorerPage::Settings);

    ImGui::SetCursorPos(position);
    ImGui::Dummy(size);
}
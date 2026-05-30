#include "ui.h"

void DrawBottomBar(
    const EditorState& state,
    float height,
    ImU32 color
)
{
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();

    ImVec2 max = ImVec2(
        windowPos.x + windowSize.x,
        windowPos.y + windowSize.y
    );

    ImVec2 min = ImVec2(
        windowPos.x,
        max.y - height
    );

    ImDrawList* draw = ImGui::GetWindowDrawList();

    draw->AddRectFilled(
        min,
        max,
        color
    );

    std::string text = state.statusMessage;

    if (HasActiveFile(state))
    {
        const EditorOpenFile* file = GetActiveOpenFile(state);

        if (file && file->dirty)
            text += "  •  Ctrl+S to save";
    }

    draw->AddText(
        ImVec2(min.x + 10.0f, min.y + 3.0f),
        IM_COL32(190, 190, 190, 255),
        text.c_str()
    );
}
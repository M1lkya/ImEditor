#pragma once

#include "imgui.h"
#include "EditorState.h"

bool DrawEditor(
    EditorState& state,
    const ImVec2& displaySize,
    ImVec2* editorMin,
    ImVec2* editorMax,
    EditorUiIntent* intent
);
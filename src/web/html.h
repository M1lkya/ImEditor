#pragma once

inline constexpr wchar_t MONACO_HTML[] = LR"HTML(
<!doctype html>
<html>
<head>
    <meta charset="utf-8" />

    <style>
        html, body {
            width: 100%;
            height: 100%;
            margin: 0;
            padding: 0;
            overflow: hidden;
            background: transparent;
            color: white;
            font-family: Consolas, monospace;
        }

        #editor-shell {
            width: 100%;
            height: 100%;
            box-sizing: border-box;
            border-radius: 8px;
            overflow: hidden;
            background: #1e1e1e;
        }

        #container {
            width: 100%;
            height: 100%;
        }
    </style>
</head>

<body>
    <div id="editor-shell">
        <div id="container"></div>
    </div>

    <script>
        let editor = null;

        function blockMonacoCommandShortcuts(event) {
            const key = event.key.toLowerCase();

            const isCtrlOrMeta = event.ctrlKey || event.metaKey;
            const isAlt = event.altKey;
            const isFunctionCommand =
                event.key === "F1" ||
                event.key === "F3";

            if (!isCtrlOrMeta && !isAlt && !isFunctionCommand)
                return;

            const allowedEditingShortcut =
                isCtrlOrMeta &&
                !event.altKey &&
                !event.shiftKey &&
                (
                    key === "c" ||
                    key === "v" ||
                    key === "x" ||
                    key === "a" ||
                    key === "z" ||
                    key === "y"
                );

            if (allowedEditingShortcut)
                return;

            event.preventDefault();
            event.stopPropagation();
        }

        document.addEventListener(
            "keydown",
            blockMonacoCommandShortcuts,
            true
        );

        function startMonaco() {
            if (typeof window.require === "undefined")
                return;

            window.require.config({
                paths: {
                    vs: "https://imeditor-assets.example/monaco/vs"
                }
            });

            window.require(["vs/editor/editor.main"], function () {
                editor = monaco.editor.create(
                    document.getElementById("container"),
                    {
                        value: `#include <iostream>

int main()
{
    std::cout << "Hello from ImEditor!" << std::endl;
    return 0;
}
`,
                        language: "cpp",
                        theme: "vs-dark",

                        automaticLayout: true,

                        fontFamily: "Cascadia Code, Consolas, monospace",
                        fontSize: 15,
                        lineHeight: 22,
                        fontLigatures: true,

                        minimap: {
                            enabled: false
                        },

                        contextmenu: false,
                        links: false,

                        hover: {
                            enabled: false
                        },

                        quickSuggestions: false,
                        suggestOnTriggerCharacters: false,

                        parameterHints: {
                            enabled: false
                        },

                        lightbulb: {
                            enabled: false
                        },

                        codeLens: false,
                        folding: false,
                        selectionHighlight: false,
                        occurrencesHighlight: "off",

                        glyphMargin: false,
                        roundedSelection: true,
                        renderLineHighlight: "gutter",

                        overviewRulerLanes: 0,
                        hideCursorInOverviewRuler: true,

                        scrollbar: {
                            verticalScrollbarSize: 8,
                            horizontalScrollbarSize: 8
                        },

                        padding: {
                            top: 14,
                            bottom: 14
                        },

                        cursorBlinking: "smooth",
                        cursorSmoothCaretAnimation: "on",
                        smoothScrolling: true
                    }
                );

                window.editor = editor;

                editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.KeyF, function () {});
                editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.KeyH, function () {});
                editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.KeyP, function () {});
                editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.KeyS, function () {});
                editor.addCommand(monaco.KeyCode.F1, function () {});
                editor.addCommand(monaco.KeyCode.F3, function () {});
            });
        }
    </script>

    <script
        src="https://imeditor-assets.example/monaco/vs/loader.js"
        onload="startMonaco();">
    </script>
</body>
</html>
)HTML";
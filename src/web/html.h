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

        #status {
            position: absolute;
            top: 12px;
            left: 12px;
            z-index: 9999;
            background: rgba(0, 0, 0, 0.75);
            padding: 8px 10px;
            border-radius: 6px;
            font-size: 13px;
            pointer-events: none;
        }
    </style>
</head>

<body>
    <div id="editor-shell">
        <div id="status">Loading loader.js...</div>
        <div id="container"></div>
    </div>

    <script>
        const statusBox = document.getElementById("status");

        window.onerror = function (message, source, line, column, error) {
            statusBox.textContent = "JS error: " + message;
        };

        function startMonaco() {
            if (typeof window.require === "undefined") {
                statusBox.textContent = "loader.js loaded, but window.require is undefined.";
                return;
            }

            statusBox.textContent = "loader.js loaded. Loading Monaco...";

            window.require.config({
                paths: {
                    vs: "https://imeditor-assets.example/monaco/vs"
                }
            });

            window.require(["vs/editor/editor.main"], function () {
                statusBox.textContent = "Monaco loaded";

                window.editor = monaco.editor.create(
                    document.getElementById("container"),
                    {
                        value: `#include <iostream>

int main()
{
    std::cout << "Hello from Monaco!" << std::endl;
    return 0;
}
`,
                        language: "cpp",
                        theme: "vs-dark",
                        automaticLayout: true,
                        minimap: {
                            enabled: false
                        }
                    }
                );

                setTimeout(function () {
                    statusBox.style.display = "none";
                }, 1000);
            }, function (error) {
                statusBox.textContent = "Monaco require failed: " + error;
            });
        }
    </script>

    <script
        src="https://imeditor-assets.example/monaco/vs/loader.js"
        onload="startMonaco();"
        onerror="document.getElementById('status').textContent = 'FAILED to load loader.js';">
    </script>
</body>
</html>
)HTML";
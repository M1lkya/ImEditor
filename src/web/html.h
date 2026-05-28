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

    <script src="https://imeditor-assets.example/monaco/vs/loader.js"></script>

    <script>
        require.config({
            paths: {
                vs: "https://imeditor-assets.example/monaco/vs"
            }
        });

        require(["vs/editor/editor.main"], function () {
            window.editor = monaco.editor.create(
                document.getElementById("container"),
                {
                    value: "#include <iostream>\\n\\nint main()\\n{\\n    std::cout << \\"Hello from Monaco!\\" << std::endl;\\n    return 0;\\n}\\n",
                    language: "cpp",
                    theme: "vs-dark",
                    automaticLayout: true,
                    minimap: {
                        enabled: false
                    }
                }
            );
        });
    </script>
</body>
</html>
)HTML";
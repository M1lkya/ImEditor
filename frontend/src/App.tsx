import { useEffect, useRef } from "react";
import * as monaco from "monaco-editor";
import "./index.css";

const imEditorTheme: monaco.editor.IStandaloneThemeData = {
  base: "vs-dark",
  inherit: true,
  rules: [
    { token: "", foreground: "D4D4D4", background: "1E1E1E" },
    { token: "comment", foreground: "7A7A7A", fontStyle: "italic" },
    { token: "keyword", foreground: "A78BFA" },
    { token: "number", foreground: "D19A66" },
    { token: "string", foreground: "98C379" },
    { token: "type", foreground: "A78BFA" },
    { token: "class", foreground: "E5C07B" },
    { token: "function", foreground: "DCDCAA" },
    { token: "variable", foreground: "D4D4D4" },
    { token: "operator", foreground: "CFCFCF" },
    { token: "delimiter", foreground: "AAAAAA" },
    { token: "predefined", foreground: "C678DD" },
    { token: "namespace", foreground: "E5C07B" },
    { token: "macro", foreground: "A78BFA" },
  ],
  colors: {
    "editor.background": "#1E1E1E",
    "editor.foreground": "#D4D4D4",

    "editorLineNumber.foreground": "#5E5E5E",
    "editorLineNumber.activeForeground": "#CFCFCF",

    "editorCursor.foreground": "#A78BFA",

    "editor.selectionBackground": "#3E3652",
    "editor.inactiveSelectionBackground": "#303030",
    "editor.selectionHighlightBackground": "#A78BFA33",

    "editor.lineHighlightBackground": "#2A2A2A",
    "editor.lineHighlightBorder": "#00000000",

    "editorIndentGuide.background1": "#333333",
    "editorIndentGuide.activeBackground1": "#555555",

    "editorWhitespace.foreground": "#3F3F3F",

    "editorBracketMatch.background": "#453A5E",
    "editorBracketMatch.border": "#A78BFA",

    "editorGutter.background": "#1E1E1E",
    "editorGutter.modifiedBackground": "#A78BFA",
    "editorGutter.addedBackground": "#98C379",
    "editorGutter.deletedBackground": "#E06C75",

    "scrollbarSlider.background": "#5A5A5A66",
    "scrollbarSlider.hoverBackground": "#6A6A6A88",
    "scrollbarSlider.activeBackground": "#7A7A7AAA",

    "editorWidget.background": "#272727",
    "editorWidget.border": "#3C3C3C",

    "editorSuggestWidget.background": "#272727",
    "editorSuggestWidget.border": "#3C3C3C",
    "editorSuggestWidget.foreground": "#D4D4D4",
    "editorSuggestWidget.selectedBackground": "#453A5E",
    "editorSuggestWidget.highlightForeground": "#A78BFA",

    "list.hoverBackground": "#333333",
    "list.activeSelectionBackground": "#453A5E",
    "list.activeSelectionForeground": "#FFFFFF",

    "input.background": "#202020",
    "input.foreground": "#D4D4D4",
    "input.border": "#3C3C3C",

    "focusBorder": "#A78BFA88",

    "editorHoverWidget.background": "#272727",
    "editorHoverWidget.border": "#3C3C3C",

    "peekView.border": "#A78BFA88",
    "peekViewEditor.background": "#1E1E1E",
    "peekViewResult.background": "#252525",

    "minimap.background": "#1E1E1E",
  },
};

export default function App() {
  const containerRef = useRef<HTMLDivElement | null>(null);
  const editorRef = useRef<monaco.editor.IStandaloneCodeEditor | null>(null);

  useEffect(() => {
    if (!containerRef.current || editorRef.current) {
      return;
    }

    monaco.editor.defineTheme("imeditor-purple", imEditorTheme);

    editorRef.current = monaco.editor.create(containerRef.current, {
      value: `#include <iostream>

int main()
{
    std::cout << "Hello from React + Monaco!" << std::endl;
    return 0;
}
`,
      language: "cpp",
      theme: "imeditor-purple",
      automaticLayout: true,

      fontFamily: "JetBrains Mono, Consolas, monospace",
      fontSize: 14,
      lineHeight: 22,

      cursorBlinking: "smooth",
      cursorSmoothCaretAnimation: "on",
      smoothScrolling: true,

      padding: {
        top: 12,
        bottom: 12,
      },

      minimap: {
        enabled: false,
      },

      contextmenu: false,
      renderLineHighlight: "line",
      roundedSelection: true,

      scrollbar: {
        verticalScrollbarSize: 10,
        horizontalScrollbarSize: 10,
      },
    });

    return () => {
      editorRef.current?.dispose();
      editorRef.current = null;
    };
  }, []);

  return (
    <div className="h-full w-full overflow-hidden rounded-lg bg-[#1e1e1e]">
      <div ref={containerRef} className="h-full w-full" />
    </div>
  );
}
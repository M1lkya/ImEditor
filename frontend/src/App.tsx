import { useEffect, useRef, useState } from "react";
import * as monaco from "monaco-editor";
import WelcomeScreen from "./welcomeScreen";
import Settings from "./Settings";
import "./index.css";

type WebViewMessageEvent = {
  data: unknown;
};

type WebViewMessageHandler = (event: WebViewMessageEvent) => void;

type WebViewBridge = {
  postMessage(message: string): void;
  addEventListener(type: "message", listener: WebViewMessageHandler): void;
  removeEventListener(type: "message", listener: WebViewMessageHandler): void;
};

type ActiveView = "none" | "welcome" | "settings" | "editor";

declare global {
  interface Window {
    chrome?: {
      webview?: WebViewBridge;
    };
  }
}

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
    "scrollbarSlider.activeBackground": "#7A7AAA",

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

    focusBorder: "#A78BFA88",

    "editorHoverWidget.background": "#272727",
    "editorHoverWidget.border": "#3C3C3C",

    "peekView.border": "#A78BFA88",
    "peekViewEditor.background": "#1E1E1E",
    "peekViewResult.background": "#252525",

    "minimap.background": "#1E1E1E",
  },
};

function sendNativeMessage(message: string) {
  window.chrome?.webview?.postMessage(message);
}

function parseSetFileMessage(message: string) {
  const prefix = "setFile\n";

  if (!message.startsWith(prefix)) {
    return null;
  }

  const titleStart = prefix.length;
  const titleEnd = message.indexOf("\n", titleStart);

  if (titleEnd < 0) {
    return null;
  }

  return {
    title: message.slice(titleStart, titleEnd),
    content: message.slice(titleEnd + 1),
  };
}

function parseSetPageMessage(message: string) {
  const prefix = "setPage\n";

  if (!message.startsWith(prefix)) {
    return null;
  }

  const page = message.slice(prefix.length).trim();

  if (
    page === "none" ||
    page === "welcome" ||
    page === "settings"
  ) {
    return page;
  }

  return null;
}

function detectLanguage(fileName: string) {
  const lower = fileName.toLowerCase();

  if (lower.endsWith(".ts") || lower.endsWith(".tsx")) {
    return "typescript";
  }

  if (lower.endsWith(".js") || lower.endsWith(".jsx") || lower.endsWith(".mjs")) {
    return "javascript";
  }

  if (lower.endsWith(".json")) {
    return "json";
  }

  if (lower.endsWith(".html") || lower.endsWith(".htm")) {
    return "html";
  }

  if (lower.endsWith(".css")) {
    return "css";
  }

  if (
    lower.endsWith(".cpp") ||
    lower.endsWith(".cc") ||
    lower.endsWith(".cxx") ||
    lower.endsWith(".c") ||
    lower.endsWith(".h") ||
    lower.endsWith(".hpp")
  ) {
    return "cpp";
  }

  if (lower.endsWith(".md")) {
    return "markdown";
  }

  return "plaintext";
}

export default function App() {
  const [activeView, setActiveView] = useState<ActiveView>("welcome");

  const containerRef = useRef<HTMLDivElement | null>(null);
  const editorRef = useRef<monaco.editor.IStandaloneCodeEditor | null>(null);
  const suppressChangeMessageRef = useRef(false);

  useEffect(() => {
    if (!containerRef.current || editorRef.current) {
      return;
    }

    monaco.editor.defineTheme("imeditor-purple", imEditorTheme);

    const editor = monaco.editor.create(containerRef.current, {
      value: "",
      language: "plaintext",
      theme: "imeditor-purple",
      automaticLayout: true,
      readOnly: true,

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

    editorRef.current = editor;

    const sendContentChanged = () => {
      if (suppressChangeMessageRef.current) {
        return;
      }

      sendNativeMessage(`contentChanged\n${editor.getValue()}`);
    };

    const contentDisposable = editor.onDidChangeModelContent(sendContentChanged);

    editor.addCommand(
      monaco.KeyMod.CtrlCmd | monaco.KeyCode.KeyS,
      () => {
        sendContentChanged();
        sendNativeMessage("saveRequested");
      }
    );

    const handleNativeMessage: WebViewMessageHandler = (event) => {
      if (typeof event.data !== "string") {
        return;
      }

      const message = event.data;

      const page = parseSetPageMessage(message);

      if (page) {
        suppressChangeMessageRef.current = true;
        editor.updateOptions({ readOnly: true });
        suppressChangeMessageRef.current = false;

        setActiveView(page);
        return;
      }

      if (message === "clearEditor") {
        suppressChangeMessageRef.current = true;
        editor.setValue("");
        editor.updateOptions({ readOnly: true });
        suppressChangeMessageRef.current = false;

        setActiveView("none");
        return;
      }

      const setFile = parseSetFileMessage(message);

      if (setFile) {
        suppressChangeMessageRef.current = true;

        editor.setValue(setFile.content);
        editor.updateOptions({ readOnly: false });

        const model = editor.getModel();

        if (model) {
          monaco.editor.setModelLanguage(model, detectLanguage(setFile.title));
        }

        suppressChangeMessageRef.current = false;

        setActiveView("editor");

        requestAnimationFrame(() => {
          editor.layout();
          editor.focus();
        });
      }
    };

    window.chrome?.webview?.addEventListener("message", handleNativeMessage);

    sendNativeMessage("ready");

    return () => {
      window.chrome?.webview?.removeEventListener("message", handleNativeMessage);

      contentDisposable.dispose();

      editor.dispose();
      editorRef.current = null;
    };
  }, []);

  return (
    <div className="h-full w-full overflow-hidden rounded-lg bg-[#1e1e1e]">
      <div
        ref={containerRef}
        className={activeView === "editor" ? "h-full w-full" : "hidden h-full w-full"}
      />

      {activeView === "welcome" && <WelcomeScreen />}
      {activeView === "settings" && <Settings />}
    </div>
  );
}
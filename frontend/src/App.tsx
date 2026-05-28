import { useEffect, useRef } from "react";
import * as monaco from "monaco-editor";
import "./index.css";

export default function App() {
  const containerRef = useRef<HTMLDivElement | null>(null);
  const editorRef = useRef<monaco.editor.IStandaloneCodeEditor | null>(null);

  useEffect(() => {
    if (!containerRef.current || editorRef.current) {
      return;
    }

    editorRef.current = monaco.editor.create(containerRef.current, {
      value: `#include <iostream>

int main()
{
    std::cout << "Hello from React + Monaco!" << std::endl;
    return 0;
}
`,
      language: "cpp",
      theme: "vs-dark",
      automaticLayout: true,
      minimap: {
        enabled: false,
      },
      contextmenu: false,
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
cmake_minimum_required(VERSION 3.20)

project(ImEditor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(NPM_EXECUTABLE
    NAMES npm.cmd npm
)

if(NOT NPM_EXECUTABLE)
    message(FATAL_ERROR "npm was not found. Install Node.js, then rerun CMake.")
endif()

find_program(POWERSHELL_EXECUTABLE
    NAMES powershell pwsh
)

if(NOT POWERSHELL_EXECUTABLE)
    message(FATAL_ERROR "PowerShell was not found.")
endif()

set(GENERATED_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/generated"
)

set(FRONTEND_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/frontend"
)

set(FRONTEND_DIST_DIR
    "${FRONTEND_DIR}/dist"
)

set(FRONTEND_BUILD_STAMP
    "${CMAKE_CURRENT_BINARY_DIR}/frontend_build.stamp"
)

file(GLOB_RECURSE FRONTEND_SOURCE_FILES CONFIGURE_DEPENDS
    "${FRONTEND_DIR}/src/*"
    "${FRONTEND_DIR}/public/*"
    "${FRONTEND_DIR}/index.html"
    "${FRONTEND_DIR}/package.json"
    "${FRONTEND_DIR}/package-lock.json"
    "${FRONTEND_DIR}/vite.config.*"
    "${FRONTEND_DIR}/tsconfig*.json"
)

add_custom_command(
    OUTPUT "${FRONTEND_BUILD_STAMP}"
    COMMAND "${NPM_EXECUTABLE}" install
    COMMAND "${NPM_EXECUTABLE}" run build
    COMMAND "${CMAKE_COMMAND}" -E touch "${FRONTEND_BUILD_STAMP}"
    WORKING_DIRECTORY "${FRONTEND_DIR}"
    DEPENDS ${FRONTEND_SOURCE_FILES}
    COMMENT "Building Vite frontend"
    VERBATIM
)

add_custom_target(BuildFrontend
    DEPENDS "${FRONTEND_BUILD_STAMP}"
)

set(EMBEDDED_FRONTEND_HEADER
    "${GENERATED_DIR}/embedded_frontend.h"
)

add_custom_command(
    OUTPUT "${EMBEDDED_FRONTEND_HEADER}"
    COMMAND "${POWERSHELL_EXECUTABLE}"
        -ExecutionPolicy Bypass
        -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed_frontend.ps1"
        -InputDir "${FRONTEND_DIST_DIR}"
        -Output "${EMBEDDED_FRONTEND_HEADER}"
    DEPENDS
        "${FRONTEND_BUILD_STAMP}"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed_frontend.ps1"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Embedding frontend assets into generated/embedded_frontend.h"
    VERBATIM
)

add_custom_target(EmbedFrontend
    DEPENDS "${EMBEDDED_FRONTEND_HEADER}"
)

add_dependencies(EmbedFrontend BuildFrontend)

set(EMBEDDED_FONT_HEADER
    "${GENERATED_DIR}/embedded_font.h"
)

add_custom_command(
    OUTPUT "${EMBEDDED_FONT_HEADER}"
    COMMAND "${POWERSHELL_EXECUTABLE}"
        -ExecutionPolicy Bypass
        -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed_font.ps1"
        -InputFile "${CMAKE_CURRENT_SOURCE_DIR}/fonts/JetBrainsMono-Regular.ttf"
        -Output "${EMBEDDED_FONT_HEADER}"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed_font.ps1"
        "${CMAKE_CURRENT_SOURCE_DIR}/fonts/JetBrainsMono-Regular.ttf"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Embedding JetBrains Mono font into generated/embedded_font.h"
    VERBATIM
)

add_custom_target(EmbedFont
    DEPENDS "${EMBEDDED_FONT_HEADER}"
)

set(EMBEDDED_CODICON_HEADER
    "${GENERATED_DIR}/embedded_codicon.h"
)

add_custom_command(
    OUTPUT "${EMBEDDED_CODICON_HEADER}"
    COMMAND "${POWERSHELL_EXECUTABLE}"
        -ExecutionPolicy Bypass
        -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed_codicon.ps1"
        -InputFile "${CMAKE_CURRENT_SOURCE_DIR}/fonts/codicon.ttf"
        -Output "${EMBEDDED_CODICON_HEADER}"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed_codicon.ps1"
        "${CMAKE_CURRENT_SOURCE_DIR}/fonts/codicon.ttf"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Embedding Codicon font into generated/embedded_codicon.h"
    VERBATIM
)

add_custom_target(EmbedCodicon
    DEPENDS "${EMBEDDED_CODICON_HEADER}"
)

set(WEBVIEW2_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/external/Microsoft.Web.WebView2"
)

set(WEBVIEW2_INCLUDE_DIR
    "${WEBVIEW2_ROOT}/build/native/include"
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(WEBVIEW2_ARCH "x64")
else()
    set(WEBVIEW2_ARCH "x86")
endif()

set(WEBVIEW2_LOADER_LIB
    "${WEBVIEW2_ROOT}/build/native/${WEBVIEW2_ARCH}/WebView2LoaderStatic.lib"
)

add_executable(ImEditor
    src/main.cpp

    src/EditorState.cpp

    src/windows/app.cpp
    src/windows/dx11/dx11_window.cpp

    src/editor.cpp

    src/ui/DrawSideBar.cpp
    src/ui/DrawBottomBar.cpp
    src/ui/DrawExplorer.cpp
    src/ui/DrawTabBar.cpp
    src/ui/DrawCodeEditor.cpp

    src/ui/components/tabs.cpp
    src/ui/components/SideBarButton.cpp

    src/web/WebViewHost.cpp

    src/core/AppPaths.cpp

    include/imgui/imgui.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/imgui_demo.cpp

    include/imgui/backends/imgui_impl_win32.cpp
    include/imgui/backends/imgui_impl_dx11.cpp
)

add_dependencies(ImEditor EmbedFrontend)
add_dependencies(ImEditor EmbedFont)
add_dependencies(ImEditor EmbedCodicon)

target_include_directories(ImEditor PRIVATE
    "${GENERATED_DIR}"

    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/backends"

    "${CMAKE_CURRENT_SOURCE_DIR}/src"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/components"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/windows"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/windows/dx11"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/web"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/core"

    "${WEBVIEW2_INCLUDE_DIR}"
)

target_link_libraries(ImEditor PRIVATE
    d3d11
    dxgi
    d3dcompiler

    shlwapi
    dwmapi

    "${WEBVIEW2_LOADER_LIB}"
)
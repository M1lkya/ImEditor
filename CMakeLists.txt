cmake_minimum_required(VERSION 3.20)

project(ImEditor)

set(GENERATED_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/generated"
)

file(GLOB_RECURSE MONACO_ASSET_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/*"
)

set(EMBEDDED_MONACO_HEADER
    "${GENERATED_DIR}/embedded_monaco.h"
)

add_custom_command(
    OUTPUT "${EMBEDDED_MONACO_HEADER}"
    COMMAND powershell
        -ExecutionPolicy Bypass
        -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed_monaco.ps1"
        -InputDir "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        -Output "${EMBEDDED_MONACO_HEADER}"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed_monaco.ps1"
        ${MONACO_ASSET_FILES}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Embedding Monaco assets into generated/embedded_monaco.h"
)

add_custom_target(EmbedMonaco
    DEPENDS "${EMBEDDED_MONACO_HEADER}"
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

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ImEditor
    src/main.cpp

    src/windows/app.cpp
    src/windows/dx11/dx11_window.cpp

    src/editor.cpp

    src/ui/DrawSideBar.cpp
    src/ui/DrawBottomBar.cpp
    src/ui/DrawExplorer.cpp
    src/ui/DrawTabBar.cpp
    src/ui/DrawCodeEditor.cpp

    src/ui/components/tabs.cpp

    src/web/MonacoHost.cpp

    include/imgui/imgui.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/imgui_demo.cpp

    include/imgui/backends/imgui_impl_win32.cpp
    include/imgui/backends/imgui_impl_dx11.cpp
)

add_dependencies(ImEditor EmbedMonaco)

target_compile_definitions(ImEditor PRIVATE
    IMEDITOR_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)

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

    "${WEBVIEW2_INCLUDE_DIR}"
)

target_link_libraries(ImEditor PRIVATE
    d3d11
    dxgi
    d3dcompiler

    shlwapi

    "${WEBVIEW2_LOADER_LIB}"
)
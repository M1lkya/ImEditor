cmake_minimum_required(VERSION 3.20)

project(ImEditor)

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

    include/imgui/imgui.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/imgui_demo.cpp

    include/imgui/backends/imgui_impl_win32.cpp
    include/imgui/backends/imgui_impl_dx11.cpp
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
)

target_link_libraries(ImEditor PRIVATE
    d3d11
    dxgi
    d3dcompiler
)
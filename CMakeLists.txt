cmake_minimum_required(VERSION 3.29)
project(NextLesson)

set(CMAKE_CXX_STANDARD 20)

# Add ImGui source files
set(IMGUI_SOURCES
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_demo.cpp
        imgui/backends/imgui_impl_dx11.cpp
        imgui/backends/imgui_impl_win32.cpp
)

# Include directories
include_directories(imgui)
include_directories(imgui/backends)

# Add your main source file
set(SOURCES
        main.cpp
        ${IMGUI_SOURCES}
        app.h
        app.cpp
)

# Create the executable
add_executable(NextLesson ${SOURCES})

# Link with DirectX and DWM libraries
target_link_libraries(NextLesson
        d3d11
        d3dcompiler
        dxguid
        dwmapi
        user32
        gdi32
        kernel32
        ole32
        oleaut32
        uuid
        winmm
        Comdlg32
)

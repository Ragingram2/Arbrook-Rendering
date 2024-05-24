createProject("engine","graphics","StaticLib")
includedirs {
    basicIncludes,
    -- "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include",
    "include/imgui/",
    "include/imgui/addons/"
}  
defines {"GLEW_STATIC"}
files { 
    "src/graphics/**.h",
    "src/graphics/**.hpp",
    "src/graphics/**.inl",
    "src/graphics/**.c",
    "src/graphics/**.cpp",
    "include/imgui/*.cpp",
    "include/imgui/addons/ImGuizmo/*.cpp",
    "include/imgui/backends/imgui_impl_glfw.cpp",
    "include/GL/src/glad.c"
}
dofile "arbrook/core/include-core.lua"

filter "configurations:*OGL"
    files {
        "include/imgui/backends/imgui_impl_opengl3.cpp",
        "include/imgui/backends/imgui_impl_opengl3.h"
    }

    removefiles {
        "include/imgui/backends/imgui_impl_dx11.cpp",
        "include/imgui/backends/imgui_impl_dx11.h"
    }

filter "configurations:*DX11"
    files {
        "include/imgui/backends/imgui_impl_dx11.cpp",
        "include/imgui/backends/imgui_impl_dx11.h"
    }

    removefiles {
        "include/imgui/backends/imgui_impl_opengl3.cpp",
        "include/imgui/backends/imgui_impl_opengl3.h"
    }

filter {}





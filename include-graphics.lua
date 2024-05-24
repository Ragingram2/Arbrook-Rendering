--This is only used to include the neccesary stuff for this module
print("including graphics")
includedirs {
    basicIncludes,
    "../../include/",
    "../../include/*/",
    "../../include/*/src/",
    "../../include/*/include/",
    "../../include/*/third_party/",
    "../../include/*/third_party/*/",
    "../../include/*/third_party/*/src/",
    "../../include/*/third_party/*/include/"
    -- "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include"
}  
dependson { "graphics" }
filter "configurations:Debug*"
    links {
        "graphics-d","glfw3_mt","libpng16",
        "GenericCodeGen-d","MachineIndependent-d","glslang-d","OGLCompiler-d","SPIRV-d",
        "OSDependent-d","glslang-default-resource-limits-d","SPIRV-Tools-d","SPIRV-Tools-opt-d",
        "spirv-cross-core-d", "spirv-cross-glsl-d"
        --"glew32s-d","opengl32"
    }

filter "configurations:Release*"
    links {
        "graphics","glfw3_mt","libpng16",
        "GenericCodeGen","MachineIndependent","glslang","OGLCompiler","SPIRV",
        "OSDependent","glslang-default-resource-limits","SPIRV-Tools","SPIRV-Tools-opt",
        "spirv-cross-core", "spirv-cross-glsl"
        --"glew32s","opengl32",
    }

filter {}


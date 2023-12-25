--This is only used to include the neccesary stuff for this module
print("including rendering")
includedirs {
    basicIncludes,
    "../../include/",
    "../../include/*/",
    "../../include/*/src/",
    "../../include/*/include/",
    "../../include/*/third_party/",
    "../../include/*/third_party/*/",
    "../../include/*/third_party/*/src/",
    "../../include/*/third_party/*/include/",
    "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include"
}  
dependson { "rendering" }
filter "configurations:Debug*"
    links {
        "rendering-d","glfw3_mt","bgfx-d","bimg-d","bx-d","glew32s","opengl32","libpng16",
        "GenericCodeGen-d","MachineIndependent-d","glslang-d","OGLCompiler-d","SPIRV-d",
        "OSDependent-d","glslang-default-resource-limits-d","SPIRV-Tools-d","SPIRV-Tools-opt-d",
        "spirv-cross-core-d", "spirv-cross-glsl-d"
    }

filter "configurations:Release*"
    links {
        "rendering","glfw3_mt","bgfx","bimg","bx","glew32s","opengl32","libpng16",
        "GenericCodeGen-d","MachineIndependent-d","glslang-d","OGLCompiler-d","SPIRV",
        "OSDependent","glslang-default-resource-limits","SPIRV-Tools","SPIRV-Tools-opt",
        "spirv-cross-core", "spirv-cross-glsl"
    }

filter {}


------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/LLGL_Build.lua
--
--  Running the build process for jsoncpp.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
print("--", "Generating project for llgl.")

newoption({
    trigger="enable-debug-layer",
    value = "BOOLEAN",
    description = "Enable the LLGL debug layer for both release and debug.",
    allowed = {
        {true,"Yes"},
        {false,"No"}
    },
    default = true
})

if not _OPTIONS["gfxapi"] then
    _OPTIONS["gfxapi"] = "OpenGL"
end

if _OPTIONS["enable-debug-layer"] then
    defines({"LLGL_ENABLE_DEBUG_LAYER"})
end

local gfxapi = _OPTIONS["gfxapi"]

if not os.ishost("windows") then
    if gfxapi == "Direct3D11"
    or gfxapi == "Direct3D12" then
        error("Direct3D11 and Direct3D12 builds are for Windows only.")
    end
end

-- Base LLGL SharedLib
group("ThirdParty")
project("LLGL")
language("C++")
cppdialect("C++11")
kind("SharedLib")

targetdir(ENGINE_BIN_OUTPUT_DIR)

includedirs({
    THIRD_PARTY_SRC_DIR,
    THIRD_PARTY_SRC_DIR.."/LLGL/include",
    THIRD_PARTY_SRC_DIR.."/glfw/deps"
})

filter({"action:vs*"})
    disablewarnings({
        "4250",
        "4251",
        "4290",
        "4103"
    })
    defines({
        "_CRT_SECURE_NO_WARNINGS",
        "_SCL_SECURE_NO_WARNINGS"
    })

filter({"configurations:Debug*"})
    defines({"_DEBUG", "LLGL_DEBUG", "LLGL_DEBUG_LAYER"})
    targetname("LLGLD")
    runtime("Debug")
filter({"configurations:Release*"})
    targetname("LLGL")
    runtime("Release")
filter({})


local host = "Win32"

if os.ishost("osx") then
end


if _OPTIONS["enable-debug-layer"] then
    group("Renderer/DebugLayer")
    files({
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/DebugLayer/**.*"
    })
end

group("Include")
files({
    "ThirdParty/LLGL/*.h"
})

group("Sources/Core")
files({
    THIRD_PARTY_SRC_DIR.."/LLGL/sources/Core/*.h",
    THIRD_PARTY_SRC_DIR.."/LLGL/sources/Core/*.cpp",
})

group("Sources/Platform")
files({
    THIRD_PARTY_SRC_DIR.."/LLGL/sources/Platform/*.h",
    THIRD_PARTY_SRC_DIR.."/LLGL/sources/Platform/*.cpp",
    THIRD_PARTY_SRC_DIR.."/LLGL/sources/Platform/"..host.."/*.h",
    THIRD_PARTY_SRC_DIR.."/LLGL/sources/Platform/"..host.."/*.cpp",
})

files({
    THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/*.h",
    THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/*.cpp"
})


-- LLGL OpenGL Shared Lib Module
if gfxapi == "OpenGL" then
    --[[filter("configurations:Debug*")
        links({ "LLGL_OpenGLD" })
    filter("configurations:Release* or Final*")
        links({ "LLGL_OpenGL" })]]

    group("ThirdParty")
    project("LLGL_OpenGL")
    language("C++")
    cppdialect("C++11")
    kind("SharedLib")

    targetdir(ENGINE_BIN_OUTPUT_DIR)
    libdirs({ENGINE_BIN_OUTPUT_DIR})

    includedirs({
        THIRD_PARTY_SRC_DIR,
        THIRD_PARTY_SRC_DIR.."/LLGL/include",
        THIRD_PARTY_SRC_DIR.."/glfw/deps"
    })

    filter({"action:vs*"})
        disablewarnings({
            "4250",
            "4251",
            "4290",
            "4103"
        })
        defines({
            "_CRT_SECURE_NO_WARNINGS",
            "_SCL_SECURE_NO_WARNINGS",
            "LLGL_GL_ENABLE_EXT_PLACEHOLDERS",
            "LLGL_GL_ENABLE_VENDOR_EXT",
            "LLGL_GL_ENABLE_DSA_EXT"
        })

    filter({"configurations:Debug*"})
        targetname("LLGL_"..gfxapi.."D")
        defines({"_DEBUG","LLGL_DEBUG"})
        links({"LLGLD"})
        runtime("Debug")

    filter({"configurations:Release* or Final*"})
        targetname("LLGL_"..gfxapi)
        links({"LLGL"})
    filter({})

    if os.ishost("windows") then
        links({
            "kernel32",
            "user32",
            "gdi32",
            "winspool",
            "shell32",
            "ole32",
            "oleaut32",
            "uuid",
            "comdlg32",
            "advapi32",
            "glu32",
            "opengl32",
            "odbc32",
            "odbccp32"
        })
    end

    group("GLCommon")
    files({
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/GLCommon/*.h",
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/GLCommon/*.cpp",
    })
    
    group("GLCommon/Texture")
    files({
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/GLCommon/Texture/*.h",
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/GLCommon/Texture/*.cpp"
    })
    
    group("OpenGL")
    files({
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/*.h",
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/*.cpp"
    })
    group("OpenGL/Buffer")
    files({
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/Buffer/*.h",
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/Buffer/*.cpp"
    })
    group("OpenGL/Ext")
    files({
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/Ext/*.h",
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/Ext/*.cpp"
    })
    group("OpenGL/Platform")
    files({
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/Platform/GLContext.h",
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/Platform/GLContext.cpp"
    })
    if host == "Win32" then
        files({
            THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/Platform/Win32/Win32GLContext.h",
            THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/Platform/Win32/Win32GLContext.cpp"
        })
    end
    -- TODO: Implement other hosts.
    group("OpenGL/RenderState")
    files({
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/RenderState/*.h",
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/RenderState/*.cpp"
    })
    group("OpenGL/Shader")
    files({
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/Shader/*.h",
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/Shader/*.cpp"
    })
    group("OpenGL/Texture")
    files({
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/Texture/*.h",
        THIRD_PARTY_SRC_DIR.."/LLGL/sources/Renderer/OpenGL/Texture/*.cpp"
    })
end
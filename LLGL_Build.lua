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

-- For LLGL_Build
newoption({
    trigger = "gfxapi",
    value = "BACKEND",
    description = "Choose which graphical backend you want to use.",
    allowed = {
        {"OpenGL",    "Use OpenGL."},
        {"Direct3D11", "Use Direct3D11 (windows only)"},
        {"Direct3D12", "Use Direct3D12 (windows only, EXPERIMENTAL)"},
        {"Vulkan",    "Use Vulkan (HIGHLY EXPERIMENTAL/BROKEN)"},
        {"Metal",     "Why do you hate yourself?"}
    },
    default = "OpenGL"
})

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

group("ThirdParty")
project("LLGL")
language("C++")
cppdialect("C++11")
kind("SharedLib")

targetdir(ENGINE_LIB_OUTPUT_DIR)

includedirs({
    "ThirdParty",
    "ThirdParty/LLGL/include",
    "ThirdParty/glfw/deps"
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
    defines({"LLGL_DEBUG", "LLGL_DEBUG_LAYER"})
    targetname("LLGL_"..gfxapi.."D")
filter({"configurations:Release*"})
    targetname("LLGL_"..gfxapi.."D")
filter({})

if _OPTIONS["enable-debug-layer"] then
    files({
        "ThirdParty/LLGL/sources/Renderer/DebugLayer/**.*"
    })
end

files({
    "ThirdParty/LLGL/Core/*.h",
    "ThirdParty/LLGL/sources/Core/*.cpp",

    "ThirdParty/LLGL/sources/Platform/*.h",
    "ThirdParty/LLGL/sources/Platform/*.cpp"
})

local host = "Win32"

files({
    "ThirdParty/LLGL/sources/Platform/"..host.."/**.h",
    "ThirdParty/LLGL/sources/Platform/"..host.."/**.cpp",
})
if os.ishost("osx") then
end

local gpath = "ThirdParty/LLGL/sources/Renderer/"..gfxapi
local cpath = "ThirdParty/LLGL/sources/Renderer"
files({
    gpath.."/*.h",
    cpath.."/*.cpp"
})

if gfxapi == "Direct3D11" or gfxapi == "Direct3D12" then
    cpath = cpath.."/DXCommon"
    files({
        gpath.."/**.h",
        gpath.."/**.cpp",
    })
end

if gfxapi == "OpenGL" then
    if os.ishost("windows") then
        links({"OpenGL32"})
    end
    cpath = cpath.."/GLCommon"
    files({
        cpath.."/**.h",
        cpath.."/**.cpp",
        gpath.."/Buffer/**.h",
        gpath.."/Buffer/**.cpp",
        gpath.."/Ext/**.h",
        gpath.."/Ext/**.cpp",
        gpath.."/Platform/*.h",
        gpath.."/Platform/*.cpp",
        gpath.."/Platform/"..host.."/**.h",
        gpath.."/Platform/"..host.."/**.cpp",
        gpath.."/RenderState/**.h",
        gpath.."/RenderState/**.cpp",
        gpath.."/Shader/**.h",
        gpath.."/Shader/**.cpp",
        gpath.."/Texture/**.h",
        gpath.."/Texture/**.cpp",
    })
end

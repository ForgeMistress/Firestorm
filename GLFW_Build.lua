------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/JSONBuild.lua
--
--  Running the build process for jsoncpp.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
print("------------------------------------------------------------------------------------------------------------------------")
print("--", "Generating project for rttr.")
print("------------------------------------------------------------------------------------------------------------------------")

group("ThirdParty")
project("GLFW")
language("C++")
cppdialect("C++17")
kind("StaticLib")

targetdir(ENGINE_LIB_OUTPUT_DIR)

if os.ishost("windows") then
    defines({"_GLFW_WIN32"})
    files({
        "ThirdParty/glfw.git/src/egl_context.h",
        "ThirdParty/glfw.git/src/glfw_config.h",
        "ThirdParty/glfw.git/src/glfw3.h",
        "ThirdParty/glfw.git/src/glfw3native.h",
        "ThirdParty/glfw.git/src/internal.h",
        "ThirdParty/glfw.git/src/mappings.h",
        "ThirdParty/glfw.git/src/osmesa_context.h",
        "ThirdParty/glfw.git/src/wgl_context.h",
        "ThirdParty/glfw.git/src/win32_joystick.h",
        "ThirdParty/glfw.git/src/win32_platform.h",

        "ThirdParty/glfw.git/src/context.c",
        "ThirdParty/glfw.git/src/egl_context.c",
        "ThirdParty/glfw.git/src/init.c",
        "ThirdParty/glfw.git/src/input.c",
        "ThirdParty/glfw.git/src/monitor.c",
        "ThirdParty/glfw.git/src/osmesa_context.c",
        "ThirdParty/glfw.git/src/vulkan.c",
        "ThirdParty/glfw.git/src/wgl_context.c",
        "ThirdParty/glfw.git/src/win32_init.c",
        "ThirdParty/glfw.git/src/win32_joystick.c",
        "ThirdParty/glfw.git/src/win32_monitor.c",
        "ThirdParty/glfw.git/src/win32_thread.c",
        "ThirdParty/glfw.git/src/win32_time.c",
        "ThirdParty/glfw.git/src/win32_window.c",
        "ThirdParty/glfw.git/src/window.c"
    })
end

includedirs({
    "ThirdParty/glfw.git/include"
})


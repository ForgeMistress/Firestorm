------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/JSONBuild.lua
--
--  Running the build process for jsoncpp.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
print("--", "Generating project for glfw.")

group("ThirdParty")
project("GLFW")
language("C++")
cppdialect("C++17")
kind("StaticLib")

targetdir(ENGINE_LIB_OUTPUT_DIR)

if os.ishost("windows") then
    defines({"_GLFW_WIN32"})
    files({
        "ThirdParty/glfw/egl_context.h",
        "ThirdParty/glfw/glfw_config.h",
        "ThirdParty/glfw/glfw3.h",
        "ThirdParty/glfw/glfw3native.h",
        "ThirdParty/glfw/internal.h",
        "ThirdParty/glfw/mappings.h",
        "ThirdParty/glfw/osmesa_context.h",
        "ThirdParty/glfw/wgl_context.h",
        "ThirdParty/glfw/win32_joystick.h",
        "ThirdParty/glfw/win32_platform.h",

        "ThirdParty/glfw/context.c",
        "ThirdParty/glfw/egl_context.c",
        "ThirdParty/glfw/init.c",
        "ThirdParty/glfw/input.c",
        "ThirdParty/glfw/monitor.c",
        "ThirdParty/glfw/osmesa_context.c",
        "ThirdParty/glfw/vulkan.c",
        "ThirdParty/glfw/wgl_context.c",
        "ThirdParty/glfw/win32_init.c",
        "ThirdParty/glfw/win32_joystick.c",
        "ThirdParty/glfw/win32_monitor.c",
        "ThirdParty/glfw/win32_thread.c",
        "ThirdParty/glfw/win32_time.c",
        "ThirdParty/glfw/win32_window.c",
        "ThirdParty/glfw/window.c"
    })
end

includedirs({
    "ThirdParty/glfw.git/include"
})

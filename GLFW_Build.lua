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
    defines({
        "_GLFW_WIN32",
        "_CRT_SECURE_NO_WARNINGS",
        "_SCL_SECURE_NO_WARNINGS"
    })
    files({
        "ThirdParty/glfw/src/egl_context.h",
        "ThirdParty/glfw/src/glfw_config.h",
        "ThirdParty/glfw/src/glfw3.h",
        "ThirdParty/glfw/src/glfw3native.h",
        "ThirdParty/glfw/src/internal.h",
        "ThirdParty/glfw/src/mappings.h",
        "ThirdParty/glfw/src/osmesa_context.h",
        "ThirdParty/glfw/src/wgl_context.h",
        "ThirdParty/glfw/src/win32_joystick.h",
        "ThirdParty/glfw/src/win32_platform.h",

        "ThirdParty/glfw/src/context.c",
        "ThirdParty/glfw/src/egl_context.c",
        "ThirdParty/glfw/src/init.c",
        "ThirdParty/glfw/src/input.c",
        "ThirdParty/glfw/src/monitor.c",
        "ThirdParty/glfw/src/osmesa_context.c",
        "ThirdParty/glfw/src/vulkan.c",
        "ThirdParty/glfw/src/wgl_context.c",
        "ThirdParty/glfw/src/win32_init.c",
        "ThirdParty/glfw/src/win32_joystick.c",
        "ThirdParty/glfw/src/win32_monitor.c",
        "ThirdParty/glfw/src/win32_thread.c",
        "ThirdParty/glfw/src/win32_time.c",
        "ThirdParty/glfw/src/win32_window.c",
        "ThirdParty/glfw/src/window.c"
    })
end

includedirs({
    "ThirdParty/glfw/include"
})

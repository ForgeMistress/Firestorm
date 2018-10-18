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
        THIRD_PARTY_SRC_DIR.."/glfw/src/egl_context.h",
        THIRD_PARTY_SRC_DIR.."/glfw/src/glfw_config.h",
        THIRD_PARTY_SRC_DIR.."/glfw/src/glfw3.h",
        THIRD_PARTY_SRC_DIR.."/glfw/src/glfw3native.h",
        THIRD_PARTY_SRC_DIR.."/glfw/src/internal.h",
        THIRD_PARTY_SRC_DIR.."/glfw/src/mappings.h",
        THIRD_PARTY_SRC_DIR.."/glfw/src/osmesa_context.h",
        THIRD_PARTY_SRC_DIR.."/glfw/src/wgl_context.h",
        THIRD_PARTY_SRC_DIR.."/glfw/src/win32_joystick.h",
        THIRD_PARTY_SRC_DIR.."/glfw/src/win32_platform.h",

        THIRD_PARTY_SRC_DIR.."/glfw/src/context.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/egl_context.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/init.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/input.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/monitor.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/osmesa_context.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/vulkan.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/wgl_context.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/win32_init.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/win32_joystick.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/win32_monitor.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/win32_thread.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/win32_time.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/win32_window.c",
        THIRD_PARTY_SRC_DIR.."/glfw/src/window.c"
    })
end
includedirs({
    THIRD_PARTY_SRC_DIR.."/glfw/include"
})

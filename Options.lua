------------------------------------------------------------------------------------------------------------------------
--
--  Options.lua
--
--  File that holds all of the options for the build.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------

-- Required for LLGL_Build.lua
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

------------------------------------------------------------------------------------------------------------------------
--
--  Options.lua
--
--  File that holds all of the options for the build.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------

newoption({
    trigger = "gfxapi",
    value = "BACKEND",
    description = "Choose which graphical backend you want to use.",
    allowed = {
        {"Vulkan", "Use Vulkan (HIGHLY EXPERIMENTAL/BROKEN)"},
    },
    default = "Vulkan"
})

newoption({
    trigger = "targetPlatform",
    value = "PLATFORM",
    description = "Choose which platform you wish to target.",
    allowed = {
        {"windows", "Target Windows."}
    },
    default = "windows"
})

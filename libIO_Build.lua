------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/libIO_Build.lua
--
--  Running the build process for libIO.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
configureEngineLib("libIO")

addDependencies({
    "libCore",
    "libMirror",
    "libJson"
})

configureUnitTest("libIO")
addDependencies({
    "libCore",
    "libMirror",
    "libJson"
})
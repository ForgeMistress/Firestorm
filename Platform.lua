------------------------------------------------------------------------------------------------------------------------
--
--  precore.lua
--
--  This file takes care of some platform specific stuff.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
local targetPlatform = _OPTIONS["targetPlatform"]

BUILD_PLATFORM = false
local knownPlatform = ""

if     os.ishost("aix")     then knownPlatform = "aix"
elseif os.ishost("bsd")     then knownPlatform = "bsd"
elseif os.ishost("haiku")   then knownPlatform = "haiku"
elseif os.ishost("linux")   then knownPlatform = "linux"
elseif os.ishost("macosx")  then knownPlatform = "macosx"
elseif os.ishost("solaris") then knownPlatform = "solaris"
elseif os.ishost("wii")     then knownPlatform = "wii"
elseif os.ishost("xbox360") then knownPlatform = "xbox360"
elseif os.ishost("windows") then
    knownPlatform = "windows"
    BUILD_PLATFORM = "windows"
end

if not BUILD_PLATFORM then
    error("can not build on this platform yet (platform: "..knownPlatform..")")
end

filter("action:vs*")
    systemversion("10.0.17134.0")
clearFilters()


if os.ishost("windows") then
    defines({ "FIRE_PLATFORM_WINDOWS" })
end

filter("action:xcode*")
    defines({ "FIRE_XCODE" })
clearFilters()

if os.ishost("macosx") then
    defines({ "FIRE_PLATFORM_OSX" })
end

filter("action:gmake*")
    defines({ "FIRE_GMAKE" })
clearFilters()

if os.ishost("linux") then
    defines({ "FIRE_PLATFORM_UNIX" })
end

if _OPTIONS["gfxapi"] == "OpenGL" then
    defines({"FIRE_GFX_OPENGL3"})
end

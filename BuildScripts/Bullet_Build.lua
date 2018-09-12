------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/Bullet_Build.lua
--
--  Running the build process for Bullet Physics.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
print("------------------------------------------------------------------------------------------------------------------------")
print("--", "Generating project for Bullet.")
print("------------------------------------------------------------------------------------------------------------------------")

group("ThirdParty/Bullet")



------------------------------------------------------------------------------------------------------------------------
-- Bullet3Collision
project("Bullet3Collision")
language("C++")
cppdialect("C++17")
kind("StaticLib")
if os.ishost("Linux") then
    buildoptions{"-fPIC"}
end
targetdir(ENGINE_LIB_OUTPUT_DIR)
includedirs({ THIRD_PARTY_SRC_DIR })

files({
    THIRD_PARTY_SRC_DIR.."/Bullet3Collision/**.h",
    THIRD_PARTY_SRC_DIR.."/Bullet3Collision/**.cpp"
})

------------------------------------------------------------------------------------------------------------------------
-- Bullet3Common
project("Bullet3Common")
language("C++")
cppdialect("C++17")
kind("StaticLib")
if os.ishost("Linux") then
    buildoptions{"-fPIC"}
end
targetdir(ENGINE_LIB_OUTPUT_DIR)
includedirs({ THIRD_PARTY_SRC_DIR })

files({
    THIRD_PARTY_SRC_DIR.."/Bullet3Common/*.h",
    THIRD_PARTY_SRC_DIR.."/Bullet3Common/*.cpp"
})


------------------------------------------------------------------------------------------------------------------------
-- Bullet3Dynamics
project("Bullet3Dynamics")
language("C++")
cppdialect("C++17")
kind("StaticLib")
if os.ishost("Linux") then
    buildoptions{"-fPIC"}
end
targetdir(ENGINE_LIB_OUTPUT_DIR)
includedirs({ THIRD_PARTY_SRC_DIR })

files({
    THIRD_PARTY_SRC_DIR.."/Bullet3Dynamics/*.h",
    THIRD_PARTY_SRC_DIR.."/Bullet3Dynamics/*.cpp"
})

------------------------------------------------------------------------------------------------------------------------
-- Bullet3Geometry
project("Bullet3Geometry")
language("C++")
cppdialect("C++17")
kind("StaticLib")
if os.ishost("Linux") then
    buildoptions{"-fPIC"}
end
targetdir(ENGINE_LIB_OUTPUT_DIR)
includedirs({ THIRD_PARTY_SRC_DIR })

files({
    THIRD_PARTY_SRC_DIR.."/Bullet3Geometry/**.h",
    THIRD_PARTY_SRC_DIR.."/Bullet3Geometry/**.cpp"
})

------------------------------------------------------------------------------------------------------------------------
-- Bullet3Dynamics
project("Bullet3Dynamics")
language("C++")
cppdialect("C++17")
kind("StaticLib")
if os.ishost("Linux") then
    buildoptions{"-fPIC"}
end
targetdir(ENGINE_LIB_OUTPUT_DIR)
includedirs({ THIRD_PARTY_SRC_DIR })

files({
    THIRD_PARTY_SRC_DIR.."/Bullet3Dynamics/*.h",
    THIRD_PARTY_SRC_DIR.."/Bullet3Dynamics/*.cpp"
})

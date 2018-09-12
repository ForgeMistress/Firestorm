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

local function configure(name, wildcard)
    project(name)
    language("C++")
    cppdialect("C++17")
    kind("StaticLib")
    if os.ishost("Linux") then
        buildoptions{"-fPIC"}
    end
    targetdir(ENGINE_LIB_OUTPUT_DIR)
    includedirs({
        THIRD_PARTY_SRC_DIR,
        THIRD_PARTY_SRC_DIR.."/bullet3/src"
    })

    files({
        THIRD_PARTY_SRC_DIR.."/bullet3/src/"..name.."/"..wildcard..".h",
        THIRD_PARTY_SRC_DIR.."/bullet3/src/"..name.."/"..wildcard..".cpp"
    })
end

configure("Bullet3Collision", "**")
configure("Bullet3Common",    "*")
configure("Bullet3Dynamics",  "*")
configure("Bullet3Geometry",  "**")
configure("Bullet3Dynamics",  "*")
configure()

------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/JSONBuild.lua
--
--  Running the build process for jsoncpp.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
configureEngineLib("libApp")

includedirs({
    "ThirdParty",
    "ThirdParty/glfw/include",
    "ThirdParty/rttr/src"
})

for _,v in ipairs(ENGINE_GAME_LIBS) do
    if v ~= "libApp" then
        addDependencies({v})
    end
end

dependson({
    "glfw"
})

------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/Source/lib/libMirror/premake5.lua
--
--  This file is part of the Elflord Engine.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
staticlib('libMirror', {"libCore"})
includedirs({
    "../../../ThirdParty/rttr/src",
    "../../../ThirdParty/rttr/build/src/"
})
filter({ "configurations:Debug*" })
    links({ "rttr_core_d" })

filter({ "configurations:Release*", "configurations:Final*" })
    links({ "rttr_core" })

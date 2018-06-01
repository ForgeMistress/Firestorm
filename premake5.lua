------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/premake5.lua
--
--  This file is part of the Elflord Engine.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------

include("precore")

workspace("ElflordPP")

staticlib('libCore')

staticlib('libMirror', {
    depends = { 'libCore', 'rttr_core' };
    incdirs = {
        "ThirdParty/rttr/src",
        "ThirdParty/rttr/build/src",
    };
    --[[post = function()
        filter({ "configurations:Debug*" });                            links({ "rttr_core_d" })
        filter({ "configurations:Release*", "configurations:Final*" }); links({ "rttr_core" })
    end;]]
})
staticlib('libExistence', {
    depends = { 
        'libCore', 
        'libMirror',
        'rttr_core'
    };
    incdirs = {
        "ThirdParty/rttr/src",
        "ThirdParty/rttr/build/src",
    };
    --[[post = function()
        filter({ "configurations:Debug*" });                            links({ "rttr_core_d" })
        filter({ "configurations:Release*", "configurations:Final*" }); links({ "rttr_core" })
    end;]]
})

staticlib('libHarnessed', {
    depends = { 'libCore' };
})


unittest('libHarnessed')
unittest('libExistence', {
    depends = { 'libMirror', 'rttr_core' };
    --[[post = function()
        links({"rttr_core"})
        --filter({ "configurations:Debug*" });                            links({ "rttr_core_d" })
        --filter({ "configurations:Release*", "configurations:Final*" }); links({ "rttr_core" })
    end;]]
})
unittest('libMirror', {
    post = function()
        filter({ "configurations:Debug*" });                            links({ "rttr_core_d" })
        filter({ "configurations:Release*", "configurations:Final*" }); links({ "rttr_core" })
    end;
})


------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/premake5.lua
--
--  This file is part of the Elflord Engine.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
--  GLOBAL SETUP
------------------------------------------------------------------------------------------------------------------------
include("GlobalConfig")
include("precore")


include("RTTRBuild")

--[[local ElflordPPEnv = createEnvironment("ElflordPP")
ElflordPPEnv:AddGlobalIncludeDirs({
	"ThirdParty/rttr/src",
	"ThirdParty/rttr/build/src"
})]]


------------------------------------------------------------------------------------------------------------------------
--  WORKSPACE DEFINITION
------------------------------------------------------------------------------------------------------------------------
workspace("ElflordPP")

------------------------------------------------------------------------------------------------------------------------
--  LIBRARIES
------------------------------------------------------------------------------------------------------------------------
staticlib('libCore')

staticlib('libMirror', {
    Dependencies = {
        'libCore',
        'libIO'
    };
    IncludeDirectories = {
        "ThirdParty/rttr/src",
        "ThirdParty/rttr/build/src"
    };
    PostArgProcessing = function()
        filter({ "configurations:Debug*" });   links({ "librttr_core_d" })
        filter({ "configurations:Release*" }); links({ "librttr_core" })
    end;
})

staticlib('libIO', {
    Dependencies = {
        'libCore',
		'libJson'
    };
    IncludeDirectories = {
        "{LibSourcePath}/libJson/json"
    };
    UsePCH = false;
})

staticlib('libMath', {
    Dependencies = {
        'libCore',
        'libMirror'
    };
})

staticlib('libJson', {
	Dependencies = {
		'libCore',
		'libMirror'
	};
	UsePCH = false;
})

staticlib('libSerial', {
	Dependencies = {
		'libCore',
		'libMirror',
		'libMath',
		'libJson',
		'libExistence',
		'libIO'
	};
})

staticlib('libExistence', {
    Dependencies = {
        'libCore',
        'libMirror'
    };
})

staticlib('libHarnessed', {
    Dependencies = {
        'libCore'
    };
})

staticlib('libScript', {
    Dependencies = {
        'libCore',
        'libMirror'
    };

    IncludeDirectories = {
        "{LibSourcePath}/{Project}/angelscript/include",
        "{LibSourcePath}/{Project}/angelscript/source"
    };

    UsePCH = false;
})




------------------------------------------------------------------------------------------------------------------------
--  UNIT TESTS
------------------------------------------------------------------------------------------------------------------------
unittest('libHarnessed')
unittest('libExistence', {
    Dependencies = { 
        'libCore',
        'libMirror'
    };
    UsePCH = false;
})

unittest('libMirror', {
    Dependencies = {
        'libCore',
        'libMirror',
        'libIO'
    };
    UsePCH = false;
})

unittest('libIO', {
    Dependencies = {
        'libCore',
        'libMirror',
        'libIO'
    };
    UsePCH = false;
})

--[[unittest('libScript', {
    Dependencies = {
        'libCore',
        'libMirror'
    };
    UsePCH = false;
})]]


########################################################################################################################
#
#  ElflordPP/CMakeLists.txt
#
#  This file is part of the Elflord Engine.
#
########################################################################################################################
#  Copyright (c) 2018 Miki Ryan
########################################################################################################################
cmake_minimum_required(VERSION 3.0)

project("ElflordPP" LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# our little cmake helper functions
include(utility)


# from rttr cmake.
if(MSVC)
    # we have to remove the default warning level,
    # otherwise we get ugly compiler warnings, because of later replacing 
    # option /W3 with /W4 (which will be later added)
    replace_compiler_option("/W3" " ") 
    if (BUILD_WITH_STATIC_RUNTIME_LIBS)
        replace_compiler_option("/MD" " ")
        replace_compiler_option("/MDd" " ")
    endif()
   
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(MINGW)
        set(GNU_STATIC_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
    else()
        set(GNU_STATIC_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CLANG_STATIC_LINKER_FLAGS "-stdlib=libc++ -static-libstdc++")
endif()



add_subdirectory(Source)

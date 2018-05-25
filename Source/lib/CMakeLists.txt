########################################################################################################################
#
#  ElflordPP/Source/CMakeLists.txt
#
#  This file is part of the Elflord Engine.
#
########################################################################################################################
#  Copyright (c) 2018 Miki Ryan
########################################################################################################################
add_subdirectory(libCore)         # Core functionality.
add_subdirectory(libExistence)    # Entity Component System.
add_subdirectory(libHarnessed)    # Test Harness library.
#add_subdirectory(libPicturesque) # 3D/2D rendering.
add_subdirectory(libMirror)       # Reflection based on the rttr library.

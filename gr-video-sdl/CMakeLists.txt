# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup dependencies
########################################################################
include(GrBoost)

find_package(SDL)

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("gr-video-sdl" ENABLE_GR_VIDEO_SDL
    SDL_FOUND
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
)

GR_SET_GLOBAL(GR_VIDEO_SDL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_VIDEO_SDL)

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_VIDEO_SDL_DESCRIPTION "GNU Radio Video SDL Blocks")

CPACK_COMPONENT("video_sdl_runtime"
    GROUP        "Video SDL"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Runtime"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("video_sdl_devel"
    GROUP        "Video SDL"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("video_sdl_python"
    GROUP        "Video SDL"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime; GRC xml files"
    DEPENDS      "runtime_python;video_sdl_runtime"
)

CPACK_COMPONENT("video_sdl_swig"
    GROUP        "Video SDL"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;video_sdl_python;video_sdl_devel"
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/video_sdl)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(grc)
    add_subdirectory(swig)
    add_subdirectory(python/video_sdl)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-video-sdl.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-video-sdl.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-video-sdl.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
    COMPONENT "video_sdl_devel"
)

endif(ENABLE_GR_VIDEO_SDL)

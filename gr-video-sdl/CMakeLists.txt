# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

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

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_VIDEO_SDL)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/video_sdl)
add_subdirectory(lib)
if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python/video_sdl)
endif(ENABLE_PYTHON)
if(ENABLE_GRC)
    add_subdirectory(grc)
endif(ENABLE_GRC)

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
)

endif(ENABLE_GR_VIDEO_SDL)

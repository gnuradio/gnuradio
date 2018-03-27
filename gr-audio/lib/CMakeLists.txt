# Copyright 2011,2013 Free Software Foundation, Inc.
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

include(GrComponent)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GR_AUDIO_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

list(APPEND gr_audio_libs gnuradio-runtime ${Boost_LIBRARIES})
list(APPEND gr_audio_sources audio_registry.cc)
list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/gr-audio.conf)

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
endif(ENABLE_GR_CTRLPORT)

########################################################################
## ALSA Support
########################################################################
find_package(ALSA)

if((ALSA_FOUND) AND NOT (CMAKE_SYSTEM_NAME STREQUAL "kFreeBSD") AND NOT (CMAKE_SYSTEM_NAME STREQUAL GNU))

    add_definitions(-DALSA_FOUND)
    GR_APPEND_SUBCOMPONENT("alsa")

    include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/alsa)
    include_directories(${ALSA_INCLUDE_DIRS})
    list(APPEND gr_audio_libs ${ALSA_LIBRARIES})
    list(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/alsa/alsa_impl.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/alsa/alsa_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/alsa/alsa_sink.cc
    )
    list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/alsa/gr-audio-alsa.conf)

endif((ALSA_FOUND) AND NOT (CMAKE_SYSTEM_NAME STREQUAL "kFreeBSD") AND NOT (CMAKE_SYSTEM_NAME STREQUAL GNU))

########################################################################
## OSS Support
########################################################################
find_package(OSS)

if(OSS_FOUND)

    add_definitions(-DOSS_FOUND)
    GR_APPEND_SUBCOMPONENT("oss")

    include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/oss)
    include_directories(${OSS_INCLUDE_DIRS})
    list(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/oss/oss_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/oss/oss_sink.cc
    )
    list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/oss/gr-audio-oss.conf)

endif(OSS_FOUND)


########################################################################
## Jack Support
########################################################################
find_package(Jack)

if(JACK_FOUND)

    add_definitions(-DJACK_FOUND)
    GR_APPEND_SUBCOMPONENT("jack")

    include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/jack)
    include_directories(${JACK_INCLUDE_DIRS})
    list(APPEND gr_audio_libs ${JACK_LIBRARIES})
    add_definitions(${JACK_DEFINITIONS})
    list(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/jack/jack_impl.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/jack/jack_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/jack/jack_sink.cc
    )
    list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/jack/gr-audio-jack.conf)

endif(JACK_FOUND)

########################################################################
## OSX Support
########################################################################
include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(AudioUnit/AudioUnit.h AUDIO_UNIT_H)
CHECK_INCLUDE_FILE_CXX(AudioToolbox/AudioToolbox.h AUDIO_TOOLBOX_H)

if(AUDIO_UNIT_H AND AUDIO_TOOLBOX_H)

    add_definitions(-DOSX_FOUND)
    GR_APPEND_SUBCOMPONENT("osx")

    set(OSX_AUDIO_VALID 1 CACHE INTERNAL "OSX Audio is valid")

    include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/osx)
    list(APPEND gr_audio_libs
        "-framework AudioUnit"
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework Carbon"
    )
    list(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/osx/osx_impl.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/osx/osx_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/osx/osx_sink.cc
    )

    list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/osx/gr-audio-osx.conf)

else(AUDIO_UNIT_H AND AUDIO_TOOLBOX_H)

    set(OSX_AUDIO_VALID 0 CACHE INTERNAL "OSX Audio is not valid")

endif(AUDIO_UNIT_H AND AUDIO_TOOLBOX_H)

########################################################################
## PortAudio Support
########################################################################
find_package(Portaudio)

if(PORTAUDIO2_FOUND)

    add_definitions(-DPORTAUDIO_FOUND)
    GR_APPEND_SUBCOMPONENT("portaudio")

    include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/portaudio)
    include_directories(${PORTAUDIO_INCLUDE_DIRS})
    list(APPEND gr_audio_libs ${PORTAUDIO_LIBRARIES})
    add_definitions(${PORTAUDIO_DEFINITIONS})
    list(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/portaudio_impl.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/portaudio_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/portaudio_sink.cc
    )
    list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/gr-audio-portaudio.conf)

endif(PORTAUDIO2_FOUND)

########################################################################
## Windows Support
########################################################################
if(WIN32)

    add_definitions(-DWIN32_FOUND)
    GR_APPEND_SUBCOMPONENT("windows")

    include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/windows)
    list(APPEND gr_audio_libs winmm.lib)
    list(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/windows_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/windows_sink.cc
    )

    #Add Windows DLL resource file if using MSVC
    IF(MSVC)
        include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-audio.rc.in
            ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-audio.rc
        @ONLY)

        list(APPEND gr_audio_sources
            ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-audio.rc
        )
    ENDIF()

endif(WIN32)

########################################################################
# Setup library
########################################################################
add_library(gnuradio-audio SHARED ${gr_audio_sources})
target_link_libraries(gnuradio-audio ${gr_audio_libs})
GR_LIBRARY_FOO(gnuradio-audio RUNTIME_COMPONENT "audio_runtime" DEVEL_COMPONENT "audio_devel")

install(FILES ${gr_audio_confs} DESTINATION ${GR_PREFSDIR} COMPONENT "audio_runtime")

if(ENABLE_STATIC_LIBS)
  if(ENABLE_GR_CTRLPORT)
    # Remove GR_CTRLPORT set this target's definitions.
    # Makes sure we don't try to use ControlPort stuff in source files
    GET_DIRECTORY_PROPERTY(STATIC_DEFS COMPILE_DEFINITIONS)
    list(REMOVE_ITEM STATIC_DEFS "GR_CTRLPORT")
    SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS "${STATIC_DEFS}")

    # readd it to the target since we removed it from the directory-wide list.
    SET_PROPERTY(TARGET gnuradio-audio APPEND PROPERTY COMPILE_DEFINITIONS "GR_CTRLPORT")
  endif(ENABLE_GR_CTRLPORT)

  add_library(gnuradio-audio_static STATIC ${gr_audio_sources})

  if(NOT WIN32)
    set_target_properties(gnuradio-audio_static
      PROPERTIES OUTPUT_NAME gnuradio-audio)
  endif(NOT WIN32)

  install(TARGETS gnuradio-audio_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "audio_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)

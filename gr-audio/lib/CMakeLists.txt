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
# Setup the include and linker paths
########################################################################
include_directories(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_AUDIO_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

list(APPEND gr_audio_libs gnuradio-core ${Boost_LIBRARIES})
list(APPEND gr_audio_sources gr_audio_registry.cc)
list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/gr-audio.conf)

########################################################################
## ALSA Support
########################################################################
find_package(ALSA)

if(ALSA_FOUND)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/alsa ${ALSA_INCLUDE_DIRS})
    list(APPEND gr_audio_libs ${ALSA_LIBRARIES})
    list(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/alsa/gri_alsa.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/alsa/audio_alsa_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/alsa/audio_alsa_sink.cc
    )
    list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/alsa/gr-audio-alsa.conf)

endif(ALSA_FOUND)

########################################################################
## OSS Support
########################################################################
find_package(OSS)

if(OSS_FOUND)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/oss ${OSS_INCLUDE_DIRS})
    list(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/oss/audio_oss_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/oss/audio_oss_sink.cc
    )
    list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/oss/gr-audio-oss.conf)

endif(OSS_FOUND)


########################################################################
## Jack Support
########################################################################
find_package(Jack)

if(JACK_FOUND)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/jack ${JACK_INCLUDE_DIRS})
    list(APPEND gr_audio_libs ${JACK_LIBRARIES})
    add_definitions(${JACK_DEFINITIONS})
    list(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/jack/gri_jack.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/jack/audio_jack_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/jack/audio_jack_sink.cc
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

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/osx)
    list(APPEND gr_audio_libs
        "-framework AudioUnit"
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework Carbon"
    )
    list(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/osx/audio_osx_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/osx/audio_osx_sink.cc
    )

endif(AUDIO_UNIT_H AND AUDIO_TOOLBOX_H)

########################################################################
## PortAudio Support
########################################################################
find_package(Portaudio)

if(PORTAUDIO_FOUND)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/portaudio ${PORTAUDIO_INCLUDE_DIRS})
    list(APPEND gr_audio_libs ${PORTAUDIO_LIBRARIES})
    add_definitions(${PORTAUDIO_DEFINITIONS})
    list(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/gri_portaudio.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/audio_portaudio_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/audio_portaudio_sink.cc
    )
    list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/gr-audio-portaudio.conf)

endif(PORTAUDIO_FOUND)

########################################################################
## Windows Support
########################################################################
if(WIN32)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/windows)
    list(APPEND gr_audio_libs winmm.lib)
    list(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/audio_windows_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/audio_windows_sink.cc
    )

endif(WIN32)

########################################################################
# Setup library
########################################################################
add_library(gnuradio-audio SHARED ${gr_audio_sources})
target_link_libraries(gnuradio-audio ${gr_audio_libs})
GR_LIBRARY_FOO(gnuradio-audio RUNTIME_COMPONENT "audio_runtime" DEVEL_COMPONENT "audio_devel")

install(FILES ${gr_audio_confs} DESTINATION ${GR_PREFSDIR} COMPONENT "audio_runtime")

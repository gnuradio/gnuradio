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
INCLUDE_DIRECTORIES(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_AUDIO_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

LIST(APPEND gr_audio_libs gnuradio-core ${Boost_LIBRARIES})
LIST(APPEND gr_audio_sources gr_audio_registry.cc)
LIST(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/gr-audio.conf)

########################################################################
## ALSA Support
########################################################################
FIND_PACKAGE(ALSA)

IF(ALSA_FOUND)

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/alsa ${ALSA_INCLUDE_DIRS})
    LIST(APPEND gr_audio_libs ${ALSA_LIBRARIES})
    LIST(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/alsa/gri_alsa.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/alsa/audio_alsa_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/alsa/audio_alsa_sink.cc
    )
    LIST(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/alsa/gr-audio-alsa.conf)

ENDIF(ALSA_FOUND)

########################################################################
## OSS Support
########################################################################
FIND_PACKAGE(OSS)

IF(OSS_FOUND)

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/oss ${OSS_INCLUDE_DIRS})
    LIST(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/oss/audio_oss_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/oss/audio_oss_sink.cc
    )
    LIST(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/oss/gr-audio-oss.conf)

ENDIF(OSS_FOUND)


########################################################################
## Jack Support
########################################################################
FIND_PACKAGE(Jack)

IF(JACK_FOUND)

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/jack ${JACK_INCLUDE_DIRS})
    LIST(APPEND gr_audio_libs ${JACK_LIBRARIES})
    ADD_DEFINITIONS(${JACK_DEFINITIONS})
    LIST(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/jack/gri_jack.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/jack/audio_jack_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/jack/audio_jack_sink.cc
    )
    LIST(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/jack/gr-audio-jack.conf)

ENDIF(JACK_FOUND)

########################################################################
## OSX Support
########################################################################
INCLUDE(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(AudioUnit/AudioUnit.h AUDIO_UNIT_H)
CHECK_INCLUDE_FILE_CXX(AudioToolbox/AudioToolbox.h AUDIO_TOOLBOX_H)

IF(AUDIO_UNIT_H AND AUDIO_TOOLBOX_H)

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/osx)
    LIST(APPEND gr_audio_libs
        -framework AudioUnit
        -framework CoreAudio
        -framework AudioToolbox
    )
    LIST(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/osx/audio_osx_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/osx/audio_osx_sink.cc
    )

ENDIF(AUDIO_UNIT_H AND AUDIO_TOOLBOX_H)

########################################################################
## PortAudio Support
########################################################################
FIND_PACKAGE(Portaudio)

IF(PORTAUDIO_FOUND)

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/portaudio ${PORTAUDIO_INCLUDE_DIRS})
    LIST(APPEND gr_audio_libs ${PORTAUDIO_LIBRARIES})
    ADD_DEFINITIONS(${PORTAUDIO_DEFINITIONS})
    LIST(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/gri_portaudio.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/audio_portaudio_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/audio_portaudio_sink.cc
    )
    LIST(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/gr-audio-portaudio.conf)

ENDIF(PORTAUDIO_FOUND)

########################################################################
## Windows Support
########################################################################
IF(WIN32)

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/windows)
    LIST(APPEND gr_audio_libs winmm.lib)
    LIST(APPEND gr_audio_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/audio_windows_source.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/audio_windows_sink.cc
    )

ENDIF(WIN32)

########################################################################
# Setup library
########################################################################
ADD_LIBRARY(gnuradio-audio SHARED ${gr_audio_sources})
TARGET_LINK_LIBRARIES(gnuradio-audio ${gr_audio_libs})
SET_TARGET_PROPERTIES(gnuradio-audio PROPERTIES DEFINE_SYMBOL "gnuradio_audio_EXPORTS")
SET_TARGET_PROPERTIES(gnuradio-audio PROPERTIES SOVERSION ${LIBVER})

INSTALL(TARGETS gnuradio-audio
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "audio_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "audio_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "audio_runtime" # .dll file
)

INSTALL(FILES ${gr_audio_confs} DESTINATION ${GR_PKG_CONF_DIR} COMPONENT "audio_runtime")

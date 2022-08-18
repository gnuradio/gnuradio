# Copyright 2011,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrComponent)

########################################################################
# Setup library
########################################################################
add_library(gnuradio-audio audio_registry.cc)
target_link_libraries(gnuradio-audio PUBLIC gnuradio-runtime)
target_include_directories(
    gnuradio-audio PUBLIC $<INSTALL_INTERFACE:include>
                          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)

list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/gr-audio.conf)

########################################################################
## ALSA Support
########################################################################
find_package(ALSA)

set(audio_deps "")

if((ALSA_FOUND)
   AND NOT (CMAKE_SYSTEM_NAME STREQUAL "kFreeBSD")
   AND NOT (CMAKE_SYSTEM_NAME STREQUAL GNU))
    target_compile_definitions(gnuradio-audio PRIVATE -DALSA_FOUND)
    target_link_libraries(gnuradio-audio PUBLIC ALSA::ALSA)
    target_sources(
        gnuradio-audio
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/alsa/alsa_impl.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/alsa/alsa_source.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/alsa/alsa_sink.cc)

    gr_append_subcomponent("alsa")
    list(APPEND audio_deps "ALSA")
    list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/alsa/gr-audio-alsa.conf)
endif()

########################################################################
## OSS Support
########################################################################
find_package(OSS)

if(OSS_FOUND)
    target_compile_definitions(gnuradio-audio PRIVATE -DOSS_FOUND)
    target_link_libraries(gnuradio-audio PUBLIC OSS::OSS)
    target_sources(gnuradio-audio PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/oss/oss_source.cc
                                          ${CMAKE_CURRENT_SOURCE_DIR}/oss/oss_sink.cc)

    gr_append_subcomponent("oss")
    list(APPEND audio_deps "OSS")
    list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/oss/gr-audio-oss.conf)
endif(OSS_FOUND)

########################################################################
## Jack Support
########################################################################
find_package(JACK)

if(JACK_FOUND)
    target_compile_definitions(gnuradio-audio PRIVATE -DJACK_FOUND)
    target_link_libraries(gnuradio-audio PUBLIC JACK::JACK)
    target_sources(
        gnuradio-audio
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/jack/jack_impl.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/jack/jack_source.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/jack/jack_sink.cc)
    gr_append_subcomponent("jack")
    list(APPEND audio_deps "JACK")
    list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/jack/gr-audio-jack.conf)
endif(JACK_FOUND)

########################################################################
## OSX Support
########################################################################
include(CheckIncludeFileCXX)
check_include_file_cxx(AudioUnit/AudioUnit.h AUDIO_UNIT_H)
check_include_file_cxx(AudioToolbox/AudioToolbox.h AUDIO_TOOLBOX_H)

if(AUDIO_UNIT_H AND AUDIO_TOOLBOX_H)
    target_compile_definitions(gnuradio-audio PRIVATE -DOSX_FOUND)
    target_link_libraries(
        gnuradio-audio PUBLIC "-framework AudioUnit" "-framework CoreAudio"
                              "-framework AudioToolbox" "-framework Carbon")
    target_sources(
        gnuradio-audio
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/osx/osx_impl.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/osx/osx_source.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/osx/osx_sink.cc)

    gr_append_subcomponent("osx")
    set(OSX_AUDIO_VALID
        1
        CACHE INTERNAL "OSX Audio is valid")
    list(APPEND gr_audio_confs ${CMAKE_CURRENT_SOURCE_DIR}/osx/gr-audio-osx.conf)
else()
    set(OSX_AUDIO_VALID
        0
        CACHE INTERNAL "OSX Audio is not valid")
endif()

########################################################################
## PortAudio Support
########################################################################
find_package(PORTAUDIO)

if(PORTAUDIO2_FOUND)
    target_compile_definitions(gnuradio-audio PRIVATE -DPORTAUDIO_FOUND)
    target_link_libraries(gnuradio-audio PUBLIC Portaudio::Portaudio)
    target_sources(
        gnuradio-audio
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/portaudio_impl.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/portaudio_source.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/portaudio_sink.cc)
    list(APPEND audio_deps "PORTAUDIO")
    gr_append_subcomponent("portaudio")
    list(APPEND gr_audio_confs
         ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/gr-audio-portaudio.conf)
endif(PORTAUDIO2_FOUND)

########################################################################
## Windows Support
########################################################################
if(WIN32)
    target_compile_definitions(gnuradio-audio PRIVATE -DWIN32_FOUND)
    target_link_libraries(gnuradio-audio PUBLIC winmm.lib)
    target_sources(
        gnuradio-audio PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/windows/windows_source.cc
                               ${CMAKE_CURRENT_SOURCE_DIR}/windows/windows_sink.cc)
    gr_append_subcomponent("windows")
    #Add Windows DLL resource file if using MSVC
    if(MSVC)
        include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-audio.rc.in
                       ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-audio.rc @ONLY)

        target_sources(gnuradio-audio
                       PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-audio.rc)
    endif()
endif(WIN32)

if(ENABLE_COMMON_PCH)
    target_link_libraries(gnuradio-audio PRIVATE common-precompiled-headers)
endif()

gr_library_foo(gnuradio-audio ${audio_deps})
install(FILES ${gr_audio_confs} DESTINATION ${GR_PREFSDIR})

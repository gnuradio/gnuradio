dnl Copyright 2011 Free Software Foundation, Inc.
dnl 
dnl This file is part of GNU Radio
dnl 
dnl GNU Radio is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3, or (at your option)
dnl any later version.
dnl 
dnl GNU Radio is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with GNU Radio; see the file COPYING.  If not, write to
dnl the Free Software Foundation, Inc., 51 Franklin Street,
dnl Boston, MA 02110-1301, USA.

AC_DEFUN([GRC_GR_AUDIO],[
    GRC_ENABLE(gr-audio)

    dnl Don't do gr-audio-alsa if gnuradio-core skipped
    GRC_CHECK_DEPENDENCY(gr-audio, gnuradio-core)

    ####################################################################
    ## ALSA Support
    ####################################################################
    GR_AUDIO_ALSA_SUPPORT=true
    dnl Don't do gr-audio-alsa if the 'alsa' package is not installed.
    PKG_CHECK_MODULES(ALSA, alsa >= 0.9,[],
        [GR_AUDIO_ALSA_SUPPORT=false;AC_MSG_RESULT([gr-audio alsa support requires package alsa, not found.])])
    AM_CONDITIONAL(GR_AUDIO_ALSA_SUPPORT, $GR_AUDIO_ALSA_SUPPORT)

    ####################################################################
    ## OSS Support
    ####################################################################
    dnl Make sure the correct library and/or headers are available.
    GR_AUDIO_OSS_SUPPORT=true
    case $host_os in
    netbsd*)
        AC_HAVE_LIBRARY(ossaudio,[],
            [GR_AUDIO_OSS_SUPPORT=false;AC_MSG_RESULT([gr-audio oss support requires library ossaudio, not found.])])
        if test $GR_AUDIO_OSS_SUPPORT != false; then
            OSS_LIBS=-lossaudio
            AC_SUBST(OSS_LIBS)
            AC_MSG_RESULT([Using OSS library $OSS_LIBS])
        fi
        ;;
    darwin*)
        dnl OSX / Darwin can't use OSS
        GR_AUDIO_OSS_SUPPORT=false
        ;;
    *)
        AC_CHECK_HEADER(sys/soundcard.h,[],
            [GR_AUDIO_OSS_SUPPORT=false;AC_MSG_RESULT([gr-audio oss support requires sys/soundcard.h, not found.])])
    esac
    AM_CONDITIONAL(GR_AUDIO_OSS_SUPPORT, $GR_AUDIO_OSS_SUPPORT)



    AC_CONFIG_FILES([ \
        gr-audio/Makefile \
        gr-audio/grc/Makefile \
        gr-audio/include/Makefile \
        gr-audio/lib/Makefile \
        gr-audio/swig/Makefile \
        gr-audio/gnuradio-audio.pc \
    ])

    GRC_BUILD_CONDITIONAL(gr-audio)
])

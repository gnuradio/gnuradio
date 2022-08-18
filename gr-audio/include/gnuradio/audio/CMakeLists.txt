# Copyright 2011,2013-2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################

set(gr_audio_install_files api.h source.h sink.h)

if(OSX_AUDIO_VALID)
    list(APPEND gr_audio_install_files osx_impl.h)
endif(OSX_AUDIO_VALID)

install(FILES ${gr_audio_install_files} DESTINATION ${GR_INCLUDE_DIR}/gnuradio/audio)

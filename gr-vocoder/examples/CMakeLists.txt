# Copyright 2011,2016,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install Examples
########################################################################
include(GrPython)

GR_PYTHON_INSTALL(
    PROGRAMS
    alaw_audio_loopback.py
    cvsd_audio_loopback.py
    g721_audio_loopback.py
    g723_24_audio_loopback.py
    g723_40_audio_loopback.py
    ulaw_audio_loopback.py
    DESTINATION ${GR_PKG_VOCODER_EXAMPLES_DIR}
)
if(LIBCODEC2_FOUND)
  GR_PYTHON_INSTALL(
    PROGRAMS
    codec2_audio_loopback.py
    DESTINATION ${GR_PKG_VOCODER_EXAMPLES_DIR}
    )
  install(
    FILES
    grfreedv.grc
    loopback-codec2.grc
    DESTINATION ${GR_PKG_VOCODER_EXAMPLES_DIR}
    )
endif(LIBCODEC2_FOUND)
if(LIBGSM_FOUND)
  GR_PYTHON_INSTALL(
    PROGRAMS
    gsm_audio_loopback.py
    DESTINATION ${GR_PKG_VOCODER_EXAMPLES_DIR}
    )
  install(
    FILES
    loopback-gsmfr.grc
    DESTINATION ${GR_PKG_VOCODER_EXAMPLES_DIR}
    )
endif(LIBGSM_FOUND)

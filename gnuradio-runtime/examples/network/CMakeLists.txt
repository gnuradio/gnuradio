# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

gr_python_install(
    PROGRAMS audio_sink.py audio_source.py dial_tone_sink.py dial_tone_source.py
             vector_sink.py vector_source.py
    DESTINATION ${GR_PKG_DATA_DIR}/examples/network)

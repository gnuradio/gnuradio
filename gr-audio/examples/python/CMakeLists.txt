# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

gr_python_install(
    PROGRAMS audio_copy.py
             audio_play.py
             audio_to_file.py
             dial_tone.py
             dial_tone_wav.py
             mono_tone.py
             multi_tone.py
             noise.py
             spectrum_inversion.py
             test_resampler.py
    DESTINATION ${GR_PKG_AUDIO_EXAMPLES_DIR})

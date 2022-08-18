# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

# Base stuff
gr_python_install(
    PROGRAMS benchmark_filters.py
             channelize.py
             chirp_channelize.py
             decimate.py
             fft_filter_ccc.py
             fir_filter_ccc.py
             fir_filter_fff.py
             gr_filtdes_api.py
             gr_filtdes_callback.py
             gr_filtdes_live_upd.py
             gr_filtdes_restrict.py
             interpolate.py
             reconstruction.py
             resampler.py
             synth_filter.py
             synth_to_chan.py
    DESTINATION ${GR_PKG_FILTER_EXAMPLES_DIR})

install(FILES filter_taps.grc filter_taps_loader.grc
              filter_taps_example_complex_bandpass_taps polyphase_channelizer_demo.grc
              resampler_demo.grc DESTINATION ${GR_PKG_FILTER_EXAMPLES_DIR})

# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

install(
  FILES
  channel_tone_response.grc
  demo_quantization.grc
  demo_two_tone.grc
  demo_spec_an.grc
  demo_gmsk.grc
  demo_qam.grc
  demo_ofdm.grc
  DESTINATION ${GR_PKG_CHANNELS_EXAMPLES_DIR}
)

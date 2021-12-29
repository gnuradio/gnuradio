# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

# Base stuff
gr_python_install(PROGRAMS fmtest.py fm_demod.py
                  DESTINATION ${GR_PKG_ANALOG_EXAMPLES_DIR})

install(FILES noise_power.grc fm_rx.grc fm_tx.grc USRP_FM_stereo.grc
              sig_source_msg_ports.grc DESTINATION ${GR_PKG_ANALOG_EXAMPLES_DIR})

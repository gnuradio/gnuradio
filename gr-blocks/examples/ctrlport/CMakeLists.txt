# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

install(FILES comparing_resamplers.grc pfb_sync_test.grc simple_copy.grc
              usrp_source_control.grc DESTINATION ${GR_PKG_DATA_DIR}/examples/ctrlport)

gr_python_install(
    PROGRAMS simple_copy_controller.py usrp_source_controller.py usrp_sink_controller.py
    DESTINATION ${GR_PKG_DATA_DIR}/examples/ctrlport)

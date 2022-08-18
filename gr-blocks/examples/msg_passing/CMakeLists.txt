# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

install(FILES strobe.grc hier/test_msg_hier.grc hier/test_msg_hier_topblock.grc
        DESTINATION ${GR_PKG_DATA_DIR}/examples/msg_passing)

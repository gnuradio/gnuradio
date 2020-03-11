# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

# Base stuff
INSTALL(FILES
    zeromq_pubsub.grc
    zeromq_pushpull.grc
    zeromq_reqrep.grc
    zmq_msg.grc
    zmq_stream.grc
    DESTINATION ${GR_PKG_ZEROMQ_EXAMPLES_DIR}
)

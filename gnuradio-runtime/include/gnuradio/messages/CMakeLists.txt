# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(FILES msg_accepter.h msg_accepter_msgq.h msg_passing.h msg_producer.h msg_queue.h
        DESTINATION ${GR_INCLUDE_DIR}/gnuradio/messages)

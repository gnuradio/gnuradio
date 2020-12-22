# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(FILES
    api.h
    port_names.h
    message_counter.h
    message_emitter.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/message
)


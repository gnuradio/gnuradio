# Copyright 2011,2013 Free Software Foundation, Inc.
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
    usrp_block.h
    usrp_source.h
    usrp_sink.h
    amsg_source.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/uhd
)

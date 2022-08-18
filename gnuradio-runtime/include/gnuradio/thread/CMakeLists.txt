# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(FILES thread.h thread_body_wrapper.h thread_group.h
        DESTINATION ${GR_INCLUDE_DIR}/gnuradio/thread)

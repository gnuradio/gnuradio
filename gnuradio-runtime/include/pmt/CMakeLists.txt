# Copyright 2010-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install the headers
########################################################################
install(FILES api.h pmt.h pmt_pool.h pmt_sugar.h pmt_serial_tags.h
        DESTINATION ${GR_INCLUDE_DIR}/pmt)

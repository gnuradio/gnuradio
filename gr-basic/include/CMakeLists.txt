# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Install header files
########################################################################
install(FILES
    gr_basic_api.h
    gr_basic_add.h
    gr_basic_add_const.h
    gr_basic_delay.h
    gr_basic_divide.h
    gr_basic_multiply.h
    gr_basic_multiply_const.h
    gr_basic_op_types.h
    gr_basic_sig_source.h
    gr_basic_stream_selector.h
    gr_basic_subtract.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "basic_devel"
)

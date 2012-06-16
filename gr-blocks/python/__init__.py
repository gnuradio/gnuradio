#
# Copyright 2012 Free Software Foundation, Inc.
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
#

'''
This is the gr-blocks package. This package provides GNU Radio
processing blocks common to many flowgraphs.
'''

from blocks_swig import *

#alias old gr_add_vXX and gr_multiply_vXX
add_vcc = add_cc
add_vff = add_ff
add_vii = add_ii
add_vss = add_ss
multiply_vcc = multiply_cc
multiply_vff = multiply_ff
multiply_vii = multiply_ii
multiply_vss = multiply_ss

#!/usr/bin/env python
#
# Copyright 2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

from gnuradio import gr
from gnuradio.eng_option import eng_option
import ecc

def main ():
	e1 = ecc.streams_encode_convolutional (100, 3, 2, [1, 0, 5, 0, 1, 6])

	e2 = ecc.streams_encode_convolutional (100, 2, 3, [1, 0, 0, 1, 5, 6])

	e3 = ecc.streams_encode_convolutional_feedback (100, 2, 3, [1, 7, 0, 1, 5, 6], [1, 1, 1, 1, 017, 017])

if __name__ == '__main__':
    main ()

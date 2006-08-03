#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
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

"""
Check Rx path or USRP Rev 1.

This configures the USRP to return a periodic sequence of integers
"""

from gnuradio import gr
from gnuradio import usrp

def build_graph ():
    rx_decim  = 32
    
    fg = gr.flow_graph ()
    usrp_rx = usrp.source_s (0, rx_decim, 1, 0x32103210, usrp.FPGA_MODE_COUNTING)
    sink = gr.check_counting_s ()
    fg.connect (usrp_rx, sink)

    # file_sink = gr.file_sink (gr.sizeof_short, 'counting.dat')
    # fg.connect (usrp_rx, file_sink)

    return fg
    
def main ():
    fg = build_graph ()
    try:
        fg.run()
    except KeyboardInterrupt:
        pass

if __name__ == '__main__':
    main ()

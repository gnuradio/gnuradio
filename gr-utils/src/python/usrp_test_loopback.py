#!/usr/bin/env python
#
# Copyright 2004,2007 Free Software Foundation, Inc.
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

"""
Digital loopback (Tx to Rx) for the USRP Rev1.
"""


from gnuradio import gr
from gnuradio import usrp


def ramp_source (tb):
    period = 2**16
    src = gr.vector_source_s (range (-period/2, period/2, 1), True)
    return src

def build_graph ():
    tx_interp =  32       # tx should be twice rx
    rx_decim  =  16
    
    tb = gr.top_block ()

    data_src = ramp_source (tb)
    # usrp_tx = usrp.sink_s (0, tx_interp, 1, 0x98)
    usrp_tx = usrp.sink_s (0, tx_interp)
    tb.connect (data_src, usrp_tx)

    usrp_rx = usrp.source_s (0, rx_decim, 1, 0x32103210, usrp.FPGA_MODE_LOOPBACK)
    sink = gr.check_counting_s ()
    tb.connect (usrp_rx, sink)

    # file_sink = gr.file_sink (gr.sizeof_short, "loopback.dat")
    # tb.connect (usrp_rx, file_sink)
    
    return tb
    
def main ():
    tb = build_graph ()
    try:
        tb.run()
    except KeyboardInterrupt:
        pass

if __name__ == '__main__':
    main ()

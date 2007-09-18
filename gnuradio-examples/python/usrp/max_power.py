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
Setup USRP for maximum power consumption.
"""


from gnuradio import gr
from gnuradio import usrp
from gnuradio.eng_option import eng_option
from optparse import OptionParser

def ramp_source ():
    period = 2**16
    src = gr.vector_source_s (range (-period/2, period/2, 1), True)
    return src

def build_block (tx_enable, rx_enable):
    max_usb_rate = 8e6                  # 8 MS/sec
    dac_freq = 128e6
    adc_freq =  64e6
    
    tx_nchan = 2
    tx_mux = 0x0000ba98
    tx_interp =  int (dac_freq / (max_usb_rate/2 * tx_nchan)) # 16
    
    rx_nchan = 2
    rx_mux = 0x00003210
    rx_decim = int ((adc_freq * rx_nchan) / (max_usb_rate/2)) # 32
    
    tb = gr.top_block ()

    if tx_enable:
        tx_src0 = gr.sig_source_c (dac_freq/tx_interp, gr.GR_CONST_WAVE, 0, 16e3, 0)
        usrp_tx = usrp.sink_c (0, tx_interp, tx_nchan, tx_mux)
        usrp_tx.set_tx_freq (0, 10e6)
        usrp_tx.set_tx_freq (1,  9e6)
        tb.connect (tx_src0, usrp_tx)

    if rx_enable:
        usrp_rx = usrp.source_c (0, rx_decim, rx_nchan, rx_mux)
        usrp_rx.set_rx_freq (0, 5.5e6)
        usrp_rx.set_rx_freq (1, 6.5e6)
        rx_dst0 = gr.null_sink (gr.sizeof_gr_complex)
        tb.connect (usrp_rx, rx_dst0)

    return tb
    
def main ():
    parser = OptionParser (option_class=eng_option)
    parser.add_option ("-t", action="store_true", dest="tx_enable",
                       default=False, help="enable Tx path")
    parser.add_option ("-r", action="store_true", dest="rx_enable",
                       default=False, help="enable Rx path")
    (options, args) = parser.parse_args ()
    tb = build_block (options.tx_enable, options.rx_enable)

    tb.start ()
    raw_input ('Press Enter to quit: ')
    tb.stop ()

if __name__ == '__main__':
    main ()

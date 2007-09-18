#!/usr/bin/env python
#
# Copyright 2004,2005 Free Software Foundation, Inc.
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
Benchmark the USB/USRP throughput.  Finds the maximum full-duplex speed
the USRP/USB combination can sustain without errors.

This program does not currently give reliable results.  Sorry about that...
"""

from gnuradio import gr
from gnuradio import usrp
from gnuradio import eng_notation

import sys

def run_test (usb_throughput, verbose):
    # usb_throughput is in bytes/sec.
    #
    # Returns True or False
    
    nsec = 1
    stream_length = int (usb_throughput/2 * nsec)   # length of stream to examine

    adc_freq =  64e6
    dac_freq = 128e6
    sizeof_sample = 2 * gr.sizeof_short

    usb_throughput_in_samples = usb_throughput / sizeof_sample

    # allocate usb throughput 50/50 between Tx and Rx

    tx_interp = int (dac_freq) / int (usb_throughput_in_samples / 2)
    rx_decim  = int (adc_freq) / int (usb_throughput_in_samples / 2)

    # print "tx_interp =", tx_interp, "rx_decim =", rx_decim
    assert (tx_interp == 2 * rx_decim)
    
    tb = gr.top_block ()

    # Build the Tx pipeline
    data_src = gr.lfsr_32k_source_s ()
    src_head = gr.head (gr.sizeof_short, int (stream_length * 2))
    usrp_tx = usrp.sink_s (0, tx_interp)
    tb.connect (data_src, src_head, usrp_tx)

    # and the Rx pipeline
    usrp_rx = usrp.source_s (0, rx_decim, 1, 0x32103210, usrp.FPGA_MODE_LOOPBACK)
    head = gr.head (gr.sizeof_short, stream_length)
    check = gr.check_lfsr_32k_s ()
    tb.connect (usrp_rx, head, check)

    tb.run ()

    ntotal = check.ntotal ()
    nright = check.nright ()
    runlength = check.runlength ()

    if verbose:
        print "usb_throughput =", eng_notation.num_to_str (usb_throughput)
        print "ntotal    =", ntotal
        print "nright    =", nright
        print "runlength =", runlength
        print "delta     =", ntotal - runlength

    return runlength >= stream_length - 80000
    
def main ():
    verbose = True
    best_rate = 0
    usb_rate = [ 2e6, 4e6, 8e6, 16e6, 32e6 ]
    #usb_rate = [ 32e6, 32e6, 32e6, 32e6, 32e6 ]
    # usb_rate.reverse ()
    for rate in usb_rate:
        sys.stdout.write ("Testing %sB/sec... " % (eng_notation.num_to_str (rate)))
        sys.stdout.flush ()
        ok = run_test (rate, verbose)
        if ok:
            best_rate = max (best_rate, rate)
            sys.stdout.write ("OK\n")
        else:
            sys.stdout.write ("FAILED\n")

    print "Max USB/USRP throughput = %sB/sec" % (eng_notation.num_to_str (best_rate),)

if __name__ == '__main__':
    main ()

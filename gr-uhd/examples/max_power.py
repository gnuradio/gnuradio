#!/usr/bin/env python
#
# Copyright 2004,2007,2011 Free Software Foundation, Inc.
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
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from optparse import OptionParser

from gnuradio import eng_notation

n2s = eng_notation.num_to_str

# Set this to a huge number; UHD will adjust to the
# maximum the USRP xxxx device can handle
MAX_RATE = 1000e6

class build_block(gr.top_block):
    def __init__(self, args, tx_enable, rx_enable):
        gr.top_block.__init__(self)

        d = uhd.find_devices(uhd.device_addr(args))
        uhd_type = d[0].get('type')

        print "\nFound '%s' at args '%s'" % \
            (uhd_type, args)

        # Test the type of USRP; if it's a USRP (v1), it has
        # 2 channels; otherwise, it has 1 channel
        if uhd_type == "usrp":
            tx_nchan = 2
            rx_nchan = 2
        else:
            tx_nchan = 1
            rx_nchan = 1
        
        if tx_enable:
            print "\nTRANSMIT CHAIN"
            stream_args = uhd.stream_args('fc32', channels=range(tx_nchan))
            self.u_tx = uhd.usrp_sink(device_addr=args, stream_args=stream_args)
            self.u_tx.set_samp_rate(MAX_RATE)

            self.tx_src0 = gr.sig_source_c(self.u_tx.get_samp_rate(),
                                           gr.GR_CONST_WAVE,
                                           0, 1.0, 0)

            # Get dboard gain range and select maximum
            tx_gain_range = self.u_tx.get_gain_range()
            tx_gain = tx_gain_range.stop()

            # Get dboard freq range and select midpoint
            tx_freq_range = self.u_tx.get_freq_range()
            tx_freq_mid = (tx_freq_range.start() + tx_freq_range.stop())/2.0

            for i in xrange(tx_nchan):
                self.u_tx.set_center_freq (tx_freq_mid + i*1e6, i)
                self.u_tx.set_gain(tx_gain, i)

            print "\nTx Sample Rate: %ssps" % (n2s(self.u_tx.get_samp_rate()))
            for i in xrange(tx_nchan):
                print "Tx Channel %d: " % (i)
                print "\tFrequency = %sHz" % \
                    (n2s(self.u_tx.get_center_freq(i)))
                print "\tGain = %f dB" % (self.u_tx.get_gain(i))
            print ""

            self.connect (self.tx_src0, self.u_tx)

        if rx_enable:
            print "\nRECEIVE CHAIN"
            self.u_rx = uhd.usrp_source(device_addr=args,
                                        io_type=uhd.io_type.COMPLEX_FLOAT32,
                                        num_channels=rx_nchan)
            self.rx_dst0 = gr.null_sink (gr.sizeof_gr_complex)

            self.u_rx.set_samp_rate(MAX_RATE)

            # Get dboard gain range and select maximum
            rx_gain_range = self.u_rx.get_gain_range()
            rx_gain = rx_gain_range.stop()

            # Get dboard freq range and select midpoint
            rx_freq_range = self.u_rx.get_freq_range()
            rx_freq_mid = (rx_freq_range.start() + rx_freq_range.stop())/2.0

            for i in xrange(tx_nchan):
                self.u_rx.set_center_freq (rx_freq_mid + i*1e6, i)
                self.u_rx.set_gain(rx_gain, i)

            print "\nRx Sample Rate: %ssps" % (n2s(self.u_rx.get_samp_rate()))
            for i in xrange(rx_nchan):
                print "Rx Channel %d: " % (i)
                print "\tFrequency = %sHz" % \
                    (n2s(self.u_rx.get_center_freq(i)))
                print "\tGain = %f dB" % (self.u_rx.get_gain(i))
            print ""

            self.connect (self.u_rx, self.rx_dst0)

def main ():
    parser = OptionParser (option_class=eng_option)
    parser.add_option("-a", "--args", type="string", default="",
                      help="UHD device address args [default=%default]")
    parser.add_option("-t", action="store_true", dest="tx_enable",
                      default=False, help="enable Tx path")
    parser.add_option("-r", action="store_true", dest="rx_enable",
                      default=False, help="enable Rx path")
    (options, args) = parser.parse_args ()

    tb = build_block (options.args, options.tx_enable, options.rx_enable)

    tb.start ()
    raw_input ('Press Enter to quit: ')
    tb.stop ()

if __name__ == '__main__':
    main ()

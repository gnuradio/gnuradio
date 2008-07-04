#!/usr/bin/env python
#
# Copyright 2006, 2007 Free Software Foundation, Inc.
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

from gnuradio import gr, blks2
from gnuradio import msdd
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import struct, sys

# from current dir
from receive_path import receive_path

class my_top_block(gr.top_block):
    def __init__(self, address, callback, options):
        gr.top_block.__init__(self)

        self._address = address
        self._rx_freq = options.rx_freq         # receiver's center frequency
        self._rx_gain = options.rx_gain         # receiver's gain
        self._decim   = options.decim           # Decimating rate for the USRP (prelim)

        if self._rx_freq is None:
            sys.stderr.write("-f FREQ or --freq FREQ or --rx-freq FREQ must be specified\n")
            raise SystemExit

        # Set up USRP source
        self._setup_source()

        #taps = gr.firdes.low_pass(1, 1, 0.4, 0.2)
        #self.resample = gr.rational_resampler_base_ccf(5, 8, taps)
        self.resample = blks2.rational_resampler_ccf(5, 8)

        # Set up receive path
        self.rxpath = receive_path(callback, options)

        self.connect(self.src, self.resample, self.rxpath)
        #self.connect(self.src, gr.file_sink(gr.sizeof_gr_complex, "receive.dat"))
        #self.connect(self.resample, gr.file_sink(gr.sizeof_gr_complex, "resampled.dat"))
        
    def _setup_source(self):
        # build graph
        self._port = 10001
        self.src = msdd.source_c(0, 1, self._address, self._port)
        self.src.set_decim_rate(self._decim)
        self.src.set_desired_packet_size(0, 1460)

        self.set_gain(self._rx_gain)
        self.set_freq(self._rx_freq)

    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool
        """
        r = self.src.set_rx_freq(0, target_freq)
        if r:
            return True
        return False

    def set_gain(self, gain):
        """
        Sets the analog gain in the USRP
        """
        return self.src.set_pga(0, gain)

    def decim(self):
        return self._decim

    def add_options(normal, expert):
        """
        Adds usrp-specific options to the Options Parser
        """
        add_freq_option(normal)
        normal.add_option("", "--rx-gain", type="eng_float", default=32, metavar="GAIN",
                          help="set receiver gain in dB [default=%default].")
        normal.add_option("-v", "--verbose", action="store_true", default=False)

        expert.add_option("", "--rx-freq", type="eng_float", default=None,
                          help="set Rx frequency to FREQ [default=%default]", metavar="FREQ")
        expert.add_option("-d", "--decim", type="intx", default=128,
                          help="set fpga decimation rate to DECIM [default=%default]")
        expert.add_option("", "--snr", type="eng_float", default=30,
                          help="set the SNR of the channel in dB [default=%default]")
   
    # Make a static method to call before instantiation
    add_options = staticmethod(add_options)

def add_freq_option(parser):
    """
    Hackery that has the -f / --freq option set both tx_freq and rx_freq
    """
    def freq_callback(option, opt_str, value, parser):
        parser.values.rx_freq = value
        parser.values.tx_freq = value

    if not parser.has_option('--freq'):
        parser.add_option('-f', '--freq', type="eng_float",
                          action="callback", callback=freq_callback,
                          help="set Tx and/or Rx frequency to FREQ [default=%default]",
                          metavar="FREQ")

# /////////////////////////////////////////////////////////////////////////////
#                                   main
# /////////////////////////////////////////////////////////////////////////////

def main():

    global n_rcvd, n_right
        
    n_rcvd = 0
    n_right = 0

    def rx_callback(ok, payload):
        global n_rcvd, n_right
        n_rcvd += 1
        (pktno,) = struct.unpack('!H', payload[0:2])
        if ok:
            n_right += 1
        print "ok: %r \t pktno: %d \t n_rcvd: %d \t n_right: %d" % (ok, pktno, n_rcvd, n_right)

        if 0:
            printlst = list()
            for x in payload[2:]:
                t = hex(ord(x)).replace('0x', '')
                if(len(t) == 1):
                    t = '0' + t
                printlst.append(t)
            printable = ''.join(printlst)

            print printable
            print "\n"

    usage = "usage: %prog [options] host"
    parser = OptionParser(usage=usage, option_class=eng_option, conflict_handler="resolve")
    expert_grp = parser.add_option_group("Expert")
    parser.add_option("","--discontinuous", action="store_true", default=False,
                      help="enable discontinuous")

    my_top_block.add_options(parser, expert_grp)
    receive_path.add_options(parser, expert_grp)
    blks2.ofdm_mod.add_options(parser, expert_grp)
    blks2.ofdm_demod.add_options(parser, expert_grp)

    (options, args) = parser.parse_args ()
    address = args[0]

    # build the graph
    tb = my_top_block(address, rx_callback, options)

    #r = gr.enable_realtime_scheduling()
    #if r != gr.RT_OK:
    #    print "Warning: failed to enable realtime scheduling"

    tb.start()                      # start flow graph
    tb.wait()                       # wait for it to finish

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass

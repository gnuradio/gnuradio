#!/usr/bin/env python
#
# Copyright 2008 Free Software Foundation, Inc.
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

from gnuradio import gr, eng_notation, usrp
from optparse import OptionParser
from gnuradio.eng_option import eng_option
from receive_path import receive_path
import gnuradio.gr.gr_threading as _threading
import sys, time

n2s = eng_notation.num_to_str

class status_thread(_threading.Thread):
    def __init__(self, tb):
        _threading.Thread.__init__(self)
        self.setDaemon(1)
        self.tb = tb
        self.done = False
        self.start()

    def run(self):
        while not self.done:
            print "Freq. Offset: %5.0f Hz  Timing Offset: %5.1f ppm  Estimated SNR: %4.1f dB  BER: %g" % (
                tb.frequency_offset(), tb.timing_offset()*1e6, tb.snr(), tb.ber())
            try:
                time.sleep(1.0)
            except KeyboardInterrupt:
                self.done = True

class rx_bpsk_block(gr.top_block):
    def __init__(self, options):    

	gr.top_block.__init__(self, "rx_mpsk")

        print "USRP decimation rate", options.decim_rate

        # Create a USRP source at desired board, sample rate, frequency, and gain
        self._setup_usrp(options.which,
                         options.decim_rate,
                         options.rx_subdev_spec,
                         options.freq,
                         options.gain)

        # Create the BERT receiver
        if_rate = self._usrp.adc_rate()/options.decim_rate
        self._receiver = receive_path(if_rate,
                                      options.rate,
                                      options.excess_bw,
                                      options.costas_alpha,
                                      options.costas_beta,
                                      options.costas_max,
                                      options.mm_gain_mu,
                                      options.mm_gain_omega,
                                      options.mm_omega_limit)
        
        self.connect(self._usrp, self._receiver)


    def _setup_usrp(self, which, decim, subdev_spec, freq, gain):
        self._usrp = usrp.source_c(which=which, decim_rate=decim)
	if subdev_spec is None:
	    subdev_spec = usrp.pick_rx_subdevice(self._usrp)
	self._subdev = usrp.selected_subdev(self._usrp, subdev_spec)
        mux = usrp.determine_rx_mux_value(self._usrp, subdev_spec)
	self._usrp.set_mux(mux)
	tr = self._usrp.tune(0, self._subdev, freq)
        if not (tr):
            print "Failed to tune to center frequency!"
        else:
            print "Center frequency:", n2s(freq)
        if gain is None:
            g = self._subdev.gain_range();
            gain = float(g[0]+g[1])/2.0
	self._subdev.set_gain(gain)
        print "RX d'board:", self._subdev.side_and_name()
        
    def snr(self):
        return self._receiver.snr()

    def mag(self):
        return self._receiver.signal_mean()
        
    def var(self):
        return self._receiver.noise_variance()

    def ber(self):
        return self._receiver.ber()

    def frequency_offset(self):
        return self._receiver.frequency_offset()
        
    def timing_offset(self):
        return self._receiver.timing_offset()
            
def get_options():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-w", "--which", type="int", default=0,
                      help="select which USRP (0, 1, ...) (default is %default)",
                      metavar="NUM")
    parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                      help="select USRP Rx side A or B (default=first one with a daughterboard)")
    parser.add_option("-f", "--freq", type="eng_float", default=None,
                      help="set frequency to FREQ", metavar="FREQ")
    parser.add_option("-g", "--gain", type="eng_float", default=None,
                      help="set Rx gain (default is mid-point)")
    parser.add_option("-r", "--rate", type="eng_float", default=250e3,
                      help="Select modulation symbol rate (default=%default)")
    parser.add_option("-d", "--decim-rate", type="int", default=8,
                      help="Select USRP decimation rate (default=%default)")
    parser.add_option("", "--excess-bw", type="eng_float", default=0.35,
                      help="Select RRC excess bandwidth (default=%default)")
    parser.add_option("", "--costas-alpha", type="eng_float", default=0.05,
                      help="set Costas loop 1st order gain, (default=%default)")
    parser.add_option("", "--costas-beta", type="eng_float", default=0.00025,
                      help="set Costas loop 2nd order gain, (default=%default)")
    parser.add_option("", "--costas-max", type="eng_float", default=0.05,
                      help="set Costas loop max freq (rad/sample) (default=%default)")
    parser.add_option("", "--mm-gain-mu", type="eng_float", default=0.001,
                      help="set M&M loop 1st order gain, (default=%default)")
    parser.add_option("", "--mm-gain-omega", type="eng_float", default=0.000001,
                      help="set M&M loop 2nd order gain, (default=%default)")
    parser.add_option("", "--mm-omega-limit", type="eng_float", default=0.0001,
                      help="set M&M max timing error, (default=%default)")

		      
    (options, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        sys.exit(1)
	
    if options.freq == None:
        print "You must supply a frequency with -f or --freq"
        sys.exit(1)

    return (options, args)


if __name__ == "__main__":
    (options, args) = get_options()

    tb = rx_bpsk_block(options)

    print "\n*** SNR estimator is inaccurate below about 7dB"
    print "*** BER estimator is inaccurate above about 10%\n"
    updater = status_thread(tb)

    try:
        tb.run()
    except KeyboardInterrupt:
        updater.done = True
        updater = None

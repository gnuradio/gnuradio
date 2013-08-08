#!/usr/bin/env python
#
# Copyright 2005-2007,2011,2012 Free Software Foundation, Inc.
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

from gnuradio import gr, audio, uhd
from gnuradio import blocks
from gnuradio import filter
from gnuradio import analog
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys

class wfm_rx_block (gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser=OptionParser(option_class=eng_option)
        parser.add_option("-a", "--args", type="string", default="",
                          help="UHD device address args [default=%default]")
        parser.add_option("", "--spec", type="string", default=None,
	                  help="Subdevice of UHD device where appropriate")
        parser.add_option("-A", "--antenna", type="string", default=None,
                          help="select Rx Antenna where appropriate")
        parser.add_option("-f", "--freq", type="eng_float", default=100.1e6,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-V", "--volume", type="eng_float", default=None,
                          help="set volume (default is midpoint)")
        parser.add_option("-O", "--audio-output", type="string", default="default",
                          help="pcm device name.  E.g., hw:0,0 or surround51 or /dev/dsp")
        parser.add_option("", "--freq-min", type="eng_float", default=87.9e6,
                          help="Set a minimum frequency [default=%default]")
        parser.add_option("", "--freq-max", type="eng_float", default=108.1e6,
                          help="Set a maximum frequency [default=%default]")

        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)

        self.state = "FREQ"
        self.freq = 0

        self.fm_freq_min = options.freq_min
        self.fm_freq_max = options.freq_max

        # build graph
        self.u = uhd.usrp_source(device_addr=options.args, stream_args=uhd.stream_args('fc32'))

        # Set the subdevice spec
        if(options.spec):
            self.u.set_subdev_spec(options.spec, 0)

        # Set the antenna
        if(options.antenna):
            self.u.set_antenna(options.antenna, 0)

        usrp_rate  = 320e3
        demod_rate = 320e3
        audio_rate = 32e3
        audio_decim = int(demod_rate / audio_rate)

        self.u.set_samp_rate(usrp_rate)
        dev_rate = self.u.get_samp_rate()

        nfilts = 32
        chan_coeffs = filter.optfir.low_pass(nfilts,           # gain
                                             nfilts*usrp_rate, # sampling rate
                                             80e3,             # passband cutoff
                                             115e3,            # stopband cutoff
                                             0.1,              # passband ripple
                                             60)               # stopband attenuation
        rrate = usrp_rate / dev_rate
        self.chan_filt = filter.pfb.arb_resampler_ccf(rrate, chan_coeffs, nfilts)

        self.guts = analog.wfm_rcv(demod_rate, audio_decim)

        self.volume_control = blocks.multiply_const_ff(1)

        # sound card as final sink
        self.audio_sink = audio.sink(int(audio_rate),
                                     options.audio_output,
                                     False)  # ok_to_block

        # now wire it all together
        self.connect(self.u, self.chan_filt, self.guts,
                     self.volume_control, self.audio_sink)

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.u.get_gain_range()
            options.gain = float(g.start()+g.stop())/2.0

        if options.volume is None:
            g = self.volume_range()
            options.volume = float(g[0]+g[1])/2

        frange = self.u.get_freq_range()
        if(frange.start() > self.fm_freq_max or frange.stop() <  self.fm_freq_min):
            sys.stderr.write("Radio does not support required frequency range.\n")
            sys.exit(1)
        if(options.freq < self.fm_freq_min or options.freq > self.fm_freq_max):
            sys.stderr.write("Requested frequency is outside of required frequency range.\n")
            sys.exit(1)

        # set initial values
        self.set_gain(options.gain)
        self.set_vol(options.volume)
        if not(self.set_freq(options.freq)):
            self._set_status_msg("Failed to set initial frequency")

    def set_vol (self, vol):
        g = self.volume_range()
        self.vol = max(g[0], min(g[1], vol))
        self.volume_control.set_k(10**(self.vol/10))
        self.update_status_bar ()

    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        Args:
            target_freq: frequency in Hz
        @rypte: bool
        """

        r = self.u.set_center_freq(target_freq)

        if r:
            self.freq = target_freq
            self.update_status_bar()
            self._set_status_msg("OK", 0)
            return True

        self._set_status_msg("Failed", 0)
        return False

    def set_gain(self, gain):
        self.u.set_gain(gain)

    def update_status_bar (self):
        msg = "Freq: %s  Volume:%f  Setting:%s" % (
            eng_notation.num_to_str(self.freq), self.vol, self.state)
        self._set_status_msg(msg, 1)

    def _set_status_msg(self, msg, which=0):
        print msg

    def volume_range(self):
        return (-20.0, 0.0, 0.5)


if __name__ == '__main__':
    tb = wfm_rx_block()
    try:
        tb.run()
    except KeyboardInterrupt:
        pass

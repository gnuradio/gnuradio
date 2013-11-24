#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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

from gnuradio import gr
from gnuradio import blocks
from gnuradio import filter
from gnuradio import analog
from gnuradio import audio
from gnuradio.filter import firdes
import sys, math

# Create a top_block
class build_graph(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        input_rate = 200e3   # rate of a broadcast FM station
        audio_rate = 44.1e3  # Rate we send the signal to the speaker

        # resample from the output of the demodulator to the rate of
        # the audio sink.
        resamp_rate = audio_rate / input_rate

        # use a file as a dummy source. Replace this with a real radio
        # receiver to capture signals over-the-air.
        src = blocks.file_source(gr.sizeof_gr_complex, "dummy.dat", True)

        # Set the demodulator using the same deviation as the receiver.
        max_dev = 75e3
        fm_demod_gain = input_rate/(2*math.pi*max_dev/8.0)
        fm_demod = analog.quadrature_demod_cf(fm_demod_gain)

        # Create a filter for the resampler and filter the audio
        # signal to 15 kHz. The nfilts is the number of filters in the
        # arbitrary resampler. It logically operates at a rate of
        # nfilts*input_rate, so we make those adjustments when
        # building the filter.
        volume = 0.20
        nfilts = 32
        resamp_taps = firdes.low_pass_2(volume*nfilts,      # gain
                                        nfilts*input_rate,  # sampling rate
                                        15e3,               # low pass cutoff freq
                                        1e3,                # width of trans. band
                                        60,                 # stop band attenuaton
                                        firdes.WIN_KAISER)

        # Build the resampler and filter
        resamp_filter = filter.pfb_arb_resampler_fff(resamp_rate,
                                                     resamp_taps, nfilts)

        # sound card as final sink You may have to add a specific
        # device name as a second argument here, something like
        # "pulse" if using pulse audio or "plughw:0,0".
        audio_sink = audio.sink(int(audio_rate))

        # now wire it all together
        self.connect(src, fm_demod)
        self.connect(fm_demod, resamp_filter)
        self.connect(resamp_filter, (audio_sink,0))

def main(args):
    tb = build_graph()
    tb.start()        # fork thread and return
    raw_input('Press Enter to quit: ')
    tb.stop()

if __name__ == '__main__':
    main(sys.argv[1:])



#!/usr/bin/env python
#
# Copyright 2004,2006,2007,2012 Free Software Foundation, Inc.
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
from gnuradio import audio
from gnuradio.eng_option import eng_option
from optparse import OptionParser

try:
    from gnuradio import analog
except ImportError:
    sys.stderr.write("Error: Program requires gr-analog.\n")
    sys.exit(1)

#import os
#print os.getpid()
#raw_input('Attach gdb and press Enter: ')

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser = OptionParser(option_class=eng_option)
        parser.add_option("-O", "--audio-output", type="string", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option("-r", "--sample-rate", type="eng_float", default=48000,
                          help="set sample rate to RATE (48000)")
        parser.add_option ("-m", "--max-channels", type="int", default="16",
                           help="set maximum channels to use")
        parser.add_option("-D", "--dont-block", action="store_false", default=True,
                          dest="ok_to_block")
        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            raise SystemExit, 1

        sample_rate = int(options.sample_rate)
        limit_channels = options.max_channels

        ampl = 0.1

        # With a tip of the hat to Harry Partch, may he R.I.P.
        # See "Genesis of a Music".  He was into some very wild tunings...
        base = 392
        ratios = { 1 : 1.0,
                   3 : 3.0/2,
                   5 : 5.0/4,
                   7 : 7.0/4,
                   9 : 9.0/8,
                   11 : 11.0/8 }

        # progression = (1, 5, 3, 7)
        # progression = (1, 9, 3, 7)
        # progression = (3, 7, 9, 11)
        # progression = (7, 11, 1, 5)
        progression = (7, 11, 1, 5, 9)

        dst = audio.sink(sample_rate,
                         options.audio_output,
                         options.ok_to_block)

        max_chan = dst.input_signature().max_streams()
        if (max_chan == -1) or (max_chan > limit_channels):
            max_chan = limit_channels

        for i in range(max_chan):
            quo, rem = divmod(i, len (progression))
            freq = base * ratios[progression[rem]] * (quo + 1)
            src = analog.sig_source_f(sample_rate, analog.GR_SIN_WAVE, freq, ampl)
            self.connect(src, (dst, i))

if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass

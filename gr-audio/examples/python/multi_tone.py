#!/usr/bin/env python
#
# Copyright 2004,2006,2007,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr
from gnuradio import audio
from gnuradio.eng_arg import eng_float, intx
from argparse import ArgumentParser

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

        parser = ArgumentParser()
        parser.add_argument("-O", "--audio-output", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_argument("-r", "--sample-rate", type=eng_float, default=48000,
                          help="set sample rate to RATE (%(default)r)")
        parser.add_argument ("-m", "--max-channels", type=int, default=16,
                           help="set maximum channels to use")
        parser.add_argument("-D", "--dont-block", action="store_false",
                          dest="ok_to_block")
        args = parser.parse_args()
        sample_rate = int(args.sample_rate)
        limit_channels = args.max_channels

        ampl = 0.1

        # With a tip of the hat to Harry Partch, may he R.I.P.
        # See "Genesis of a Music".  He was into some very wild tunings...
        base = 392
        ratios = { 1 : 1.0,
                   3 : 3.0 / 2,
                   5 : 5.0 / 4,
                   7 : 7.0 / 4,
                   9 : 9.0 / 8,
                   11 : 11.0 / 8 }

        # progression = (1, 5, 3, 7)
        # progression = (1, 9, 3, 7)
        # progression = (3, 7, 9, 11)
        # progression = (7, 11, 1, 5)
        progression = (7, 11, 1, 5, 9)

        dst = audio.sink(sample_rate,
                         args.audio_output,
                         args.ok_to_block)

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

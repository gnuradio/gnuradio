#!/usr/bin/env python
#
# Copyright 2005 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

from gnuradio import gr, blks
from gnuradio import audio
from gnuradio.eng_option import eng_option
from optparse import OptionParser

class my_graph(gr.flow_graph):

    def __init__(self):
        gr.flow_graph.__init__(self)

        parser = OptionParser(option_class=eng_option)
        parser.add_option("-I", "--audio-input", type="string", default="",
                          help="pcm input device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option("-O", "--audio-output", type="string", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        (options, args) = parser.parse_args ()
        if len(args) != 0:
            parser.print_help()
            raise SystemExit, 1

        sample_rate = 8000
        src = audio.source(sample_rate, options.audio_input)
        tx = blks.digital_voice_tx(self)
        if_gain = gr.multiply_const_cc(10000)
        # channel simulator here...
        rx = blks.digital_voice_rx(self)
        dst = audio.sink(sample_rate, options.audio_output)

        self.connect(src, tx, if_gain, rx, dst)


if __name__ == '__main__':
    try:
        my_graph().run()
    except KeyboardInterrupt:
        pass

#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
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
from gnuradio import audio
from gnuradio.eng_option import eng_option
from optparse import OptionParser

def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-I", "--audio-input", type="string", default="",
                      help="pcm input device name.  E.g., hw:0,0 or /dev/dsp")
    parser.add_option("-O", "--audio-output", type="string", default="",
                      help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
    parser.add_option("-r", "--sample-rate", type="eng_float", default="32000",
                      help="Audio sampling rate [defaul=%default]")
    parser.add_option("-S", "--resample-rate", type="int", default="8",
                      help="Resampling rate in CVSD [default=%default]")
    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help()
        raise SystemExit, 1

    tb = gr.top_block()

    src = audio.source(int(options.sample_rate), options.audio_input)
    tx = blks2.cvsd_encode(options.resample_rate)

    # todo: add noise

    rx = blks2.cvsd_decode(options.resample_rate)
    dst = audio.sink(int(options.sample_rate), options.audio_output)
    
    tb.connect(src, tx, rx, dst)    
    tb.run()
    
if __name__ == '__main__':
    print "Enter CTRL-C to exit"
    try:
        main()
    except KeyboardInterrupt:
        pass
    

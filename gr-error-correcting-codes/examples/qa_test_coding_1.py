#!/usr/bin/env python
#
# Copyright 2004,2005 Free Software Foundation, Inc.
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

from gnuradio import gr, audio
from gnuradio.eng_option import eng_option
from optparse import OptionParser
# must be imported from local directory so that make check
# can run before installation
import ecc

class my_graph(gr.flow_graph):

    def __init__(self):
        gr.flow_graph.__init__(self)

        parser = OptionParser(option_class=eng_option)
        parser.add_option("-O", "--audio-output", type="string", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option("-r", "--sample-rate", type="eng_float", default=48000,
                          help="set sample rate to RATE (48000)")
        (options, args) = parser.parse_args ()
        if len(args) != 0:
            parser.print_help()
            raise SystemExit, 1

        sample_rate = int(options.sample_rate)
	audio_option = options.audio_output
        src = audio.source (sample_rate, audio_option)
	src_out_chan = src.output_signature().max_streams()
        dst = audio.sink (sample_rate, str(src_out_chan))
	dst_in_chan = dst.input_signature().max_streams()
	audio_el_size = src.output_signature().sizeof_stream_item(1)
	frame_size = 1000
	do_mux_outputs = 0
	enc_code_in_chan = src_out_chan
	code_generators = [05, 06] #, 03, 04] # , 0, 07]
	enc_code_out_chan = len (code_generators) / enc_code_in_chan
	do_termination = 1

	if do_mux_outputs == 1:
	    enc_dec_chan = 1
	else:
	    enc_dec_chan = enc_code_out_chan

	ss_enc = ecc.streams_encode_convolutional (frame_size,
						   enc_code_in_chan,
						   enc_code_out_chan,
						   code_generators,
						   do_termination)
# for now
	ss2s = gr.streams_to_stream (1, enc_dec_chan);
	ns = gr.null_sink (1);
# end for now

#	ss_dec = error-correcting-codes.frames_to_streams (audio_el_size, 1, 1000)

	for i in range (src_out_chan):
	    self.connect ((src, i), (ss_enc, i))
# for now
	for i in range (enc_dec_chan):
	    self.connect ((ss_enc, i), (ss2s, i))
	self.connect (ss2s, ns)
# end for now

#	for i in range (enc_dec_chan):
#	    self.connect ((ss_enc, i), (ss_dec, i))

#	for i in range (dst_in_chan):
#	    self.connect ((ss_dec, i), (dst, i))

if __name__ == '__main__':
    try:
        my_graph().run()
    except KeyboardInterrupt:
        pass

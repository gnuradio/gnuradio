#!/usr/bin/env python
#
# Copyright 2006 Free Software Foundation, Inc.
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
	enc_chan_in = 2
	src_out_chan = src.output_signature().max_streams()
        dst = audio.sink (sample_rate, str(enc_chan_in))
	dst_in_chan = dst.input_signature().max_streams()
	audio_el_size = dst.input_signature().sizeof_stream_item(0)
	frame_size = 10
	enc_code_in_chan = enc_chan_in
	code_generators = [05, 06] #, 03, 04] #, 0, 07]
	enc_code_out_chan = len (code_generators) / enc_code_in_chan
	do_termination = 1

	ss_enc = ecc.streams_encode_convolutional (frame_size,
						   enc_code_in_chan,
						   enc_code_out_chan,
						   code_generators,
						   do_termination)

	for i in range (src_out_chan):
	    p2up = gr.packed_to_unpacked_bb (1, 1)
	    self.connect ((src, i), p2up, (ss_enc, i))

	tau_bits = 0  # 0 -> decode all first
	ss_dec = error-correcting-codes.dec_blk_conv_soft_full (audio_el_size,
					     frame_size,
					     do_termination,
					     do_mux_outputs,
					     enc_code_in_chan,
					     enc_code_out_chan,
					     code_generators)
        up2bf0 = gr.chunks_to_symbols_bf ([1.0, -1.0])
        self.connect ((ss_enc, 0), p2up0, up2bf0, (ss_dec, 0))
        if enc_dec_chan > 1:
	    p2up1 = gr.packed_to_unpacked_bb (1, 1)
	    up2bf1 = gr.chunks_to_symbols_bf ([1.0, -1.0])
	    self.connect ((ss_enc, 1), p2up1, up2bf1, (ss_dec, 1))

	for i in range (dst_in_chan):
	    self.connect ((ss_dec, i), (dst, i))

if __name__ == '__main__':
    try:
        my_graph().run()
    except KeyboardInterrupt:
        pass

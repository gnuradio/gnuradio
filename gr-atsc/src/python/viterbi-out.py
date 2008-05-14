#!/usr/bin/env python
#
# Copyright 2004,2006,2007 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

from gnuradio import gr, atsc
import sys, os

def main(args):

	print os.getpid()

	nargs = len (args)
	if nargs == 1:
	   outfile = args[0]
	else:
	   sys.stderr.write ('usage: viterbi_out.py output_file\n')
	   sys.exit (1)

	tb = gr.top_block()

        src = gr.file_source(atsc.sizeof_atsc_soft_data_segment, "/tmp/atsc_pipe_5")
	viterbi = atsc.viterbi_decoder()
        deinter = atsc.deinterleaver()
        rs_dec = atsc.rs_decoder()
        derand = atsc.derandomizer()
	depad = atsc.depad()
        dst = gr.file_sink(gr.sizeof_char,outfile)
	tb.connect(src, viterbi, deinter, rs_dec, derand, depad, dst)
        tb.run ()

        
if __name__ == '__main__':
    main(sys.argv[1:])







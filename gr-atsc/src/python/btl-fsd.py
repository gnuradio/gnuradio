#!/usr/bin/env python
#
# Copyright 2004,2005,2007 Free Software Foundation, Inc.
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

from gnuradio import gr
from gnuradio import atsc
import os

print os.getpid()

tb = gr.top_block()

btl = atsc.bit_timing_loop()
fsc = atsc.fs_checker()
eq = atsc.equalizer()
fsd = atsc.field_sync_demux()

out_data = gr.file_sink(atsc.sizeof_atsc_soft_data_segment,"/tmp/atsc_pipe_5")

inp = gr.file_source(gr.sizeof_float,"/tmp/atsc_pipe_3")

tb.connect(inp,btl)
tb.connect((btl,0),(fsc,0),(eq,0),(fsd,0))
tb.connect((btl,1),(fsc,1),(eq,1),(fsd,1))
tb.connect(fsd,out_data)

tb.run()



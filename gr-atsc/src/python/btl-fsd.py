#!/usr/bin/python

from gnuradio import gr
from gnuradio import atsc

fg = gr.flow_graph()

btl = atsc.bit_timing_loop()
fsc = atsc.fs_checker()
eq = atsc.equalizer()
fsd = atsc.field_sync_demux()

out_data = gr.file_sink(atsc.sizeof_atsc_soft_data_segment,"/tmp/atsc_pipe_5")

inp = gr.file_source(gr.sizeof_float,"/tmp/atsc_pipe_3")

fg.connect(inp,btl)
fg.connect((btl,0),(fsc,0))
fg.connect((btl,1),(fsc,1))
fg.connect((fsc,0),(eq,0))
fg.connect((fsc,1),(eq,1))
fg.connect((eq,0),(fsd,0))
fg.connect((eq,1),(fsd,1))
fg.connect(fsd,out_data)

fg.run()



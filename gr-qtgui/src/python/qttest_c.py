#!/usr/bin/env python

from gnuradio import gr
from gnuradio.qtgui import qtgui

class my_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        fftsize = 2048

        src1 = gr.sig_source_c(1, gr.GR_SIN_WAVE, 0.1, 0.01, 0)
        src2 = gr.sig_source_c(1, gr.GR_SIN_WAVE, 0.015, 0.01, 0)
        src  = gr.add_cc()
        thr = gr.throttle(gr.sizeof_gr_complex, 20*fftsize)
        self.snk = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS, -0.5, 0.5)

        self.connect(src1, (src,0))
        self.connect(src2, (src,1))
        self.connect(src,  thr, self.snk)
        
if __name__ == "__main__":
    tb = my_top_block();
    tb.start()
    tb.snk.start_app();
    #tb.wait();
    

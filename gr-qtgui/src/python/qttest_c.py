#!/usr/bin/env python

from gnuradio import gr
from gnuradio.qtgui import qtgui

class my_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        Rs = 8000
        f1 = 1000
        f2 = 2000

        fftsize = 2048

        src1 = gr.sig_source_c(Rs, gr.GR_SIN_WAVE, f1, 0.1, 0)
        src2 = gr.sig_source_c(Rs, gr.GR_SIN_WAVE, f2, 0.1, 0)
        src  = gr.add_cc()
        thr = gr.throttle(gr.sizeof_gr_complex, 20*fftsize)
        self.snk1 = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS, -Rs/2, Rs/2)
        self.snk2 = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS, -Rs/2, Rs/2)

        self.connect(src1, (src,0))
        self.connect(src2, (src,1))
        self.connect(src,  thr, self.snk1)
        self.connect(src1, self.snk2)
        
        self.snk1.initialize()
        qapp = self.snk1.get_qapplication()
        self.snk2.initialize(qapp)

if __name__ == "__main__":
    tb = my_top_block();
    tb.start()
    tb.snk1.start_app();
    #tb.wait();
    

#!/usr/bin/env python

from gnuradio import gr, blks2
from gnuradio.qtgui import qtgui
import scipy

class my_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        sps = 2
        excess_bw = 0.35
        gray_code = True
        
        fftsize = 2048

        data = scipy.random.randint(0, 255, 1000)
        src = gr.vector_source_b(data, True)
        mod = blks2.dqpsk_mod(sps, excess_bw, gray_code, False, False)

        rrctaps = gr.firdes.root_raised_cosine(1, sps, 1, excess_bw, 21)
        rx_rrc = gr.fir_filter_ccf(sps, rrctaps)
        
        thr = gr.throttle(gr.sizeof_gr_complex, 10*fftsize)
        self.snk_tx = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS, -1/2, 1/2)
        self.snk_rx = qtgui.sink_c(fftsize, gr.firdes.WIN_BLACKMAN_hARRIS, -1/2, 1/2)

        self.connect(src, mod, self.snk_tx)
        self.connect(mod, rx_rrc, thr, self.snk_rx)
        
        self.snk_tx.initialize()
        qapp = self.snk_tx.get_qapplication()
        self.snk_rx.initialize(qapp)

if __name__ == "__main__":
    tb = my_top_block();
    tb.start()
    tb.snk_tx.start_app();
    #tb.wait();
    

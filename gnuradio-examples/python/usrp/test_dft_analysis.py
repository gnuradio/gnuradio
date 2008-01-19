#!/usr/bin/env python

from gnuradio import gr, gru, blks2
from gnuradio.wxgui import stdgui2, fftsink2, slider
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import wx

class test_graph (stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__(self, frame, panel, vbox, argv)

        parser = OptionParser (option_class=eng_option)
        (options, args) = parser.parse_args ()

        sample_rate = 16e3
        mpoints = 4
        ampl = 1000
        freq = 0

        lo_freq = 1e6
        lo_ampl = 1
        
        vbox.Add(slider.slider(panel,
                               -sample_rate/2, sample_rate/2,
                               self.set_lo_freq), 0, wx.ALIGN_CENTER)


        src = gr.sig_source_c(sample_rate, gr.GR_CONST_WAVE,
                              freq, ampl, 0)

        self.lo = gr.sig_source_c(sample_rate, gr.GR_SIN_WAVE,
                                  lo_freq, lo_ampl, 0)

        mixer = gr.multiply_cc()
        self.connect(src, (mixer, 0))
        self.connect(self.lo, (mixer, 1))
        
        # We add these throttle blocks so that this demo doesn't
        # suck down all the CPU available.  Normally you wouldn't use these.
        thr = gr.throttle(gr.sizeof_gr_complex, sample_rate)

        taps = gr.firdes.low_pass(1,   # gain
                                  1,   # rate
                                  1.0/mpoints * 0.4,  # cutoff
                                  1.0/mpoints * 0.1,  # trans width
                                  gr.firdes.WIN_HANN)
        print len(taps)
        analysis = blks2.analysis_filterbank(mpoints, taps)
        
        self.connect(mixer, thr)
        self.connect(thr, analysis)

        for i in range(mpoints):
            fft = fftsink2.fft_sink_c(frame, fft_size=128,
                                     sample_rate=sample_rate/mpoints,
                                     fft_rate=5,
                                     title="Ch %d" % (i,))
            self.connect((analysis, i), fft)
            vbox.Add(fft.win, 1, wx.EXPAND)

    def set_lo_freq(self, freq):
        self.lo.set_frequency(freq)
        
                                     

def main ():
    app = stdgui2.stdapp (test_graph, "Test DFT filterbank")
    app.MainLoop ()

if __name__ == '__main__':
    main ()

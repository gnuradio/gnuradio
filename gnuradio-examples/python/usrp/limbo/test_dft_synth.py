#!/usr/bin/env python

from gnuradio import gr, gru, blks
from gnuradio.wxgui import stdgui, fftsink
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import wx
import random


def make_random_complex_tuple(L, gain=1):
    result = []
    for x in range(L):
        result.append(gain * complex(random.gauss(0, 1),random.gauss(0, 1)))
                      
    return tuple(result)

def random_noise_c(gain=1):
    src = gr.vector_source_c(make_random_complex_tuple(32*1024, gain), True)
    return src


class test_graph (stdgui.gui_flow_graph):
    def __init__(self, frame, panel, vbox, argv):
        stdgui.gui_flow_graph.__init__(self, frame, panel, vbox, argv)

        parser = OptionParser (option_class=eng_option)
        (options, args) = parser.parse_args ()

        sample_rate = 16e6
        mpoints = 16
        ampl = 1000
        
        enable = mpoints * [0]
        enable[0] = 1

        taps = gr.firdes.low_pass(1,   # gain
                                  1,   # rate
                                  1.0/mpoints * 0.4,  # cutoff
                                  1.0/mpoints * 0.1,  # trans width
                                  gr.firdes.WIN_HANN)

        synth = blks.synthesis_filterbank(self, mpoints, taps)
        
        null_source = gr.null_source(gr.sizeof_gr_complex)
        
        if 0:
            for i in range(mpoints):
                s = gr.sig_source_c(sample_rate/mpoints, gr.GR_SIN_WAVE,
                                    300e3, ampl * enable[i], 0)
                self.connect(s, (synth, i))

        else:
            for i in range(mpoints):
                if i == 0:
                    s = gr.sig_source_c(sample_rate/mpoints, gr.GR_SIN_WAVE,
                                        300e3, ampl * enable[i], 0)
                    #s = random_noise_c(ampl)
                    self.connect(s, (synth, i))
                else:
                    self.connect(null_source, (synth, i))
            

        # We add these throttle blocks so that this demo doesn't
        # suck down all the CPU available.  Normally you wouldn't use these.
        thr = gr.throttle(gr.sizeof_gr_complex, sample_rate)
        fft = fftsink.fft_sink_c(self, frame, fft_size=1024,
                                 sample_rate=sample_rate)
        vbox.Add(fft.win, 1, wx.EXPAND)

        self.connect(synth, thr, fft)


def main ():
    app = stdgui.stdapp (test_graph, "Test DFT filterbank")
    app.MainLoop ()

if __name__ == '__main__':
    main ()

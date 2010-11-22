#!/usr/bin/env python

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio import window
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser

class uhd_burst_detector(gr.top_block):
    def __init__(self, frequency, sample_rate,
                 uhd_address="192.168.10.2"):

        gr.top_block.__init__(self)

        self.freq = frequency
        self.samp_rate = sample_rate
        self.uhd_addr = uhd_address
        self.gain = 32
        
        self.uhd_src = uhd.single_usrp_source(
            device_addr="addr="+self.uhd_addr,
            io_type=uhd.io_type_t.COMPLEX_FLOAT32,
            num_channels=1,
            )
        
        self.uhd_src.set_samp_rate(self.samp_rate)
        self.uhd_src.set_center_freq(self.freq, 0)
        self.uhd_src.set_gain(self.gain, 0)

        taps = firdes.low_pass_2(1, 1, 0.4, 0.1, 60)
        self.chanfilt = gr.fir_filter_ccc(10, taps)
        self.ann0 = gr.annotator_alltoall(100000, gr.sizeof_gr_complex)
        self.tagger = gr.burst_tagger(gr.sizeof_gr_complex)

        # Dummy signaler to collect a burst on known periods
        data = 1000*[0,] + 1000*[1,]
        self.signal = gr.vector_source_s(data, True)

        # Energy detector to get signal burst
        self.c2m = gr.complex_to_mag_squared()
        self.iir = gr.single_pole_iir_filter_ff(0.0001)
        self.sub = gr.sub_ff()
        self.mult = gr.multiply_const_ff(32768)
        self.f2s = gr.float_to_short()
        self.fsnk = gr.tagged_file_sink(gr.sizeof_gr_complex, self.samp_rate)
        

        ##################################################
        # Connections
        ##################################################
        self.connect((self.uhd_src, 0), (self.tagger, 0))
        self.connect((self.tagger, 0), (self.fsnk, 0))

        if 0:
            # Connect a dummy signaler to the burst tagger
            self.connect((self.signal, 0), (self.tagger, 1))

        else:
            # Connect an energy detector signaler to the burst tagger
            self.connect((self.uhd_src, 0), (self.c2m, 0))
            self.connect((self.c2m, 0), (self.sub, 0))
            self.connect((self.c2m, 0), (self.iir, 0))
            self.connect((self.iir, 0), (self.sub, 1))
            self.connect((self.sub, 0), (self.mult,0))
            self.connect((self.mult, 0), (self.f2s, 0))
            self.connect((self.f2s, 0), (self.tagger, 1))
        
    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.wxgui_fftsink2_0.set_sample_rate(self.samp_rate/10)
        self.uhd_src_0.set_samp_rate(self.samp_rate)
            
if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    
    frequency = 450e6
    samp_rate = samp_rate = 200000
    uhd_addr = "192.168.10.2"

    tb = uhd_burst_detector(frequency, samp_rate, uhd_addr)
    tb.run()

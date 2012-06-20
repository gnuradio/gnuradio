#!/usr/bin/env python

from gnuradio import gr, filter
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

try:
    import scipy
except ImportError:
    print "Error: could not import scipy (http://www.scipy.org/)"
    sys.exit(1)

try:
    import pylab
except ImportError:
    print "Error: could not import pylab (http://matplotlib.sourceforge.net/)"
    sys.exit(1)

class example_fir_filter_fff(gr.top_block):
    def __init__(self, N, fs, bw, tw, atten, D):
        gr.top_block.__init__(self)

        self._nsamps = N
        self._fs = fs
        self._bw = bw
        self._tw = tw
        self._at = atten
        self._decim = D
        taps = filter.firdes.low_pass_2(1, self._fs, self._bw, self._tw, self._at)
        print "Num. Taps: ", len(taps)

        self.src  = gr.noise_source_f(gr.GR_GAUSSIAN, 1)
        self.head = gr.head(gr.sizeof_float, self._nsamps)

        self.filt0 = filter.fir_filter_fff(self._decim, taps)

        self.vsnk_src = gr.vector_sink_f()
        self.vsnk_out = gr.vector_sink_f()

        self.connect(self.src, self.head, self.vsnk_src)
        self.connect(self.head, self.filt0, self.vsnk_out)

def main():
    parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
    parser.add_option("-N", "--nsamples", type="int", default=10000,
                      help="Number of samples to process [default=%default]")
    parser.add_option("-s", "--samplerate", type="eng_float", default=8000,
                      help="System sample rate [default=%default]")
    parser.add_option("-B", "--bandwidth", type="eng_float", default=1000,
                      help="Filter bandwidth [default=%default]")
    parser.add_option("-T", "--transition", type="eng_float", default=100,
                      help="Transition band [default=%default]")
    parser.add_option("-A", "--attenuation", type="eng_float", default=80,
                      help="Stopband attenuation [default=%default]")
    parser.add_option("-D", "--decimation", type="int", default=1,
                      help="Decmation factor [default=%default]")
    (options, args) = parser.parse_args ()

    put = example_fir_filter_fff(options.nsamples,
                                 options.samplerate,
                                 options.bandwidth,
                                 options.transition,
                                 options.attenuation,
                                 options.decimation)
    put.run()

    data_src = scipy.array(put.vsnk_src.data())
    data_snk = scipy.array(put.vsnk_out.data())

    # Plot the signals PSDs
    nfft = 1024
    f1 = pylab.figure(1, figsize=(12,10))
    s1 = f1.add_subplot(1,1,1)
    s1.psd(data_src, NFFT=nfft, noverlap=nfft/4,
           Fs=options.samplerate)
    s1.psd(data_snk, NFFT=nfft, noverlap=nfft/4,
           Fs=options.samplerate)

    f2 = pylab.figure(2, figsize=(12,10))
    s2 = f2.add_subplot(1,1,1)
    s2.plot(data_src)
    s2.plot(data_snk.real, 'g')

    pylab.show()
    
if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass


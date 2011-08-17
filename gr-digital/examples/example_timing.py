#!/usr/bin/env python

from gnuradio import gr, digital
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

from scipy import fftpack

class example_timing(gr.top_block):
    def __init__(self, N, sps, rolloff, ntaps, bw, noise,
                 foffset, toffset, poffset, mode=0):
        gr.top_block.__init__(self)

        rrc_taps = gr.firdes.root_raised_cosine(
            sps, sps, 1.0, rolloff, ntaps)

        gain = 2*scipy.pi/100.0
        nfilts = 32
        rrc_taps_rx = gr.firdes.root_raised_cosine(
            nfilts, sps*nfilts, 1.0, rolloff, ntaps*nfilts)
            
        data = 2.0*scipy.random.randint(0, 2, N) - 1.0
        data = scipy.exp(1j*poffset) * data

        self.src = gr.vector_source_c(data.tolist(), False)
        self.rrc = gr.interp_fir_filter_ccf(sps, rrc_taps)
        self.chn = gr.channel_model(noise, foffset, toffset)
        self.off = gr.fractional_interpolator_cc(0.20, 1.0)

        if mode == 0:
            self.clk = gr.pfb_clock_sync_ccf(sps, gain, rrc_taps_rx,
                                             nfilts, nfilts//2, 3.5)
            self.taps = self.clk.get_taps()
            self.dtaps = self.clk.get_diff_taps()

            self.vsnk_err = gr.vector_sink_f()
            self.vsnk_rat = gr.vector_sink_f()
            self.vsnk_phs = gr.vector_sink_f()

            self.connect((self.clk,1), self.vsnk_err)
            self.connect((self.clk,2), self.vsnk_rat)
            self.connect((self.clk,3), self.vsnk_phs)
            
        else: # mode == 1
            mu = 0.5
            gain_mu = 0.1
            gain_omega = 0.25*gain_mu*gain_mu
            omega_rel_lim = 0.02
            self.clk = digital.clock_recovery_mm_cc(sps, gain_omega,
                                                    mu, gain_mu,
                                                    omega_rel_lim)

            self.vsnk_err = gr.vector_sink_f()

            self.connect((self.clk,1), self.vsnk_err)

        self.vsnk_src = gr.vector_sink_c()
        self.vsnk_clk = gr.vector_sink_c()

        self.connect(self.src, self.rrc, self.chn, self.off, self.clk, self.vsnk_clk)
        self.connect(self.off, self.vsnk_src)


def main():
    parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
    parser.add_option("-N", "--nsamples", type="int", default=2000,
                      help="Set the number of samples to process [default=%default]")
    parser.add_option("-S", "--sps", type="int", default=4,
                      help="Set the samples per symbol [default=%default]")
    parser.add_option("-r", "--rolloff", type="eng_float", default=0.35,
                      help="Set the rolloff factor [default=%default]")
    parser.add_option("-W", "--bandwidth", type="eng_float", default=2*scipy.pi/100.0,
                      help="Set the loop bandwidth [default=%default]")
    parser.add_option("-n", "--ntaps", type="int", default=45,
                      help="Set the number of taps in the filters [default=%default]")
    parser.add_option("", "--noise", type="eng_float", default=0.0,
                      help="Set the simulation noise voltage [default=%default]")
    parser.add_option("-f", "--foffset", type="eng_float", default=0.0,
                      help="Set the simulation's normalized frequency offset (in Hz) [default=%default]")
    parser.add_option("-t", "--toffset", type="eng_float", default=1.0,
                      help="Set the simulation's timing offset [default=%default]")
    parser.add_option("-p", "--poffset", type="eng_float", default=0.0,
                      help="Set the simulation's phase offset [default=%default]")
    parser.add_option("-M", "--mode", type="int", default=0,
                      help="Set the recovery mode (0: polyphase, 1: M&M) [default=%default]")
    (options, args) = parser.parse_args ()

    # Adjust N for the interpolation by sps
    options.nsamples = options.nsamples // options.sps

    # Set up the program-under-test
    put = example_timing(options.nsamples, options.sps, options.rolloff,
                         options.ntaps, options.bandwidth, options.noise,
                         options.foffset, options.toffset, options.poffset,
                         options.mode)
    put.run()

    if options.mode == 0:
        data_src = scipy.array(put.vsnk_src.data()[20:])
        data_clk = scipy.array(put.vsnk_clk.data()[20:])

        data_err = scipy.array(put.vsnk_err.data()[20:])
        data_rat = scipy.array(put.vsnk_rat.data()[20:])
        data_phs = scipy.array(put.vsnk_phs.data()[20:])

        f1 = pylab.figure(1, figsize=(12,10), facecolor='w')

        # Plot the IQ symbols
        s1 = f1.add_subplot(2,2,1)
        s1.plot(data_src.real, data_src.imag, "bo")
        s1.plot(data_clk.real, data_clk.imag, "ro")
        s1.set_title("IQ")
        s1.set_xlabel("Real part")
        s1.set_ylabel("Imag part")
        s1.set_xlim([-2, 2])
        s1.set_ylim([-2, 2])

        # Plot the symbols in time
        s2 = f1.add_subplot(2,2,2)
        s2.plot(data_src.real, "bo-")
        s2.plot(data_clk.real, "ro")
        s2.set_title("Symbols")
        s2.set_xlabel("Samples")
        s2.set_ylabel("Real Part of Signals")

        # Plot the clock recovery loop's error
        s3 = f1.add_subplot(2,2,3)
        s3.plot(data_err)
        s3.set_title("Clock Recovery Loop Error")
        s3.set_xlabel("Samples")
        s3.set_ylabel("Error")

        # Plot the clock recovery loop's error
        s4 = f1.add_subplot(2,2,4)
        s4.plot(data_phs)
        s4.set_title("Clock Recovery Loop Filter Phase")
        s4.set_xlabel("Samples")
        s4.set_ylabel("Filter Phase")


        diff_taps = put.dtaps
        ntaps = len(diff_taps[0])
        nfilts = len(diff_taps)
        t = scipy.arange(0, ntaps*nfilts)

        f3 = pylab.figure(3, figsize=(12,10), facecolor='w')
        s31 = f3.add_subplot(2,1,1)
        s32 = f3.add_subplot(2,1,2)
        s31.set_title("Differential Filters")
        s32.set_title("FFT of Differential Filters")

        for i,d in enumerate(diff_taps):
            D = 20.0*scipy.log10(abs(fftpack.fftshift(fftpack.fft(d, 10000))))
            s31.plot(t[i::nfilts].real, d, "-o")
            s32.plot(D)

    # If testing the M&M clock recovery loop
    else:
        data_src = scipy.array(put.vsnk_src.data()[20:])
        data_clk = scipy.array(put.vsnk_clk.data()[20:])

        data_err = scipy.array(put.vsnk_err.data()[20:])

        f1 = pylab.figure(1, figsize=(12,10), facecolor='w')
        
        # Plot the IQ symbols
        s1 = f1.add_subplot(2,2,1)
        s1.plot(data_src.real, data_src.imag, "o")
        s1.plot(data_clk.real, data_clk.imag, "ro")
        s1.set_title("IQ")
        s1.set_xlabel("Real part")
        s1.set_ylabel("Imag part")
        s1.set_xlim([-2, 2])
        s1.set_ylim([-2, 2])

        # Plot the symbols in time
        s2 = f1.add_subplot(2,2,2)
        s2.plot(data_src.real, "o-")
        s2.plot(data_clk.real, "ro")
        s2.set_title("Symbols")
        s2.set_xlabel("Samples")
        s2.set_ylabel("Real Part of Signals")

        # Plot the clock recovery loop's error
        s3 = f1.add_subplot(2,2,3)
        s3.plot(data_err)
        s3.set_title("Clock Recovery Loop Error")
        s3.set_xlabel("Samples")
        s3.set_ylabel("Error")

    pylab.show()
    
if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass


#!/usr/bin/env python

from gnuradio import gr, blks2
import scipy, pylab

def main():
    N = 1000000
    fs = 8000

    #freqs = [100, 200, 300, 400, 500]
    freqs = [100,]
    nchans = 7

    sigs = list()
    fmtx = list()
    for fi in freqs:
        s = gr.sig_source_c(fs, gr.GR_SIN_WAVE, fi, 1)
        fm = blks2.nbfm_tx (fs, 4*fs, max_dev=10000, tau=75e-6)
        sigs.append(s)
        fmtx.append(fm)

    taps = gr.firdes.low_pass_2(len(freqs), fs, fs/float(nchans)/2, 100, 100)
    print "Num. Taps = %d (taps per filter = %d)" % (len(taps), 
                                                     len(taps)/nchans)
    #filtbank = blks2.synthesis_filterbank(nchans, taps)
    filtbank = gr.pfb_synthesis_filterbank_ccf(nchans, taps)

    head = gr.head(gr.sizeof_gr_complex, N)
    snk = gr.vector_sink_c()

    tb = gr.top_block()
    tb.connect(filtbank, head, snk)

    for i,si in enumerate(sigs):
        #tb.connect(si, fmtx[i], (filtbank, i))
        tb.connect(si, (filtbank, i))
    
    tb.run()

    if 0:
        f1 = pylab.figure(1)
        s1 = f1.add_subplot(1,1,1)
        s1.plot(snk.data()[1000:])
        
        fftlen = 2048
        f2 = pylab.figure(2)
        s2 = f2.add_subplot(1,1,1)
        winfunc = scipy.blackman
        #winfunc = scipy.hamming
        s2.psd(snk.data()[10000:], NFFT=fftlen,
               Fs = nchans*fs,
               noverlap=fftlen/4,
               window = lambda d: d*winfunc(fftlen))

        pylab.show()

if __name__ == "__main__":
    main()

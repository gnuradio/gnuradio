#!/usr/bin/env python

import scipy, math, pylab
from scipy import fftpack
from gnuradio import gr, digital, blks2

fftlen = 8192

def main():
    N = 10000
    fs = 2000.0
    Ts = 1.0/fs
    t = scipy.arange(0, N*Ts, Ts)

    # When playing with the number of channels, be careful about the filter
    # specs and the channel map of the synthesizer set below.
    nchans = 10

    # Build the filter(s)
    bw = 1000
    tb = 400
    proto_taps = gr.firdes.low_pass_2(1, nchans*fs, bw, tb, 80,
                                      gr.firdes.WIN_BLACKMAN_hARRIS)
    print "Filter length: ", len(proto_taps)


    # Create a modulated signal
    npwr = 0.01
    data = scipy.random.randint(0, 256, N)
    rrc_taps = gr.firdes.root_raised_cosine(1, 2, 1, 0.35, 41)

    src = gr.vector_source_b(data.astype(scipy.uint8).tolist(), False)
    mod = digital.bpsk_mod(samples_per_symbol=2)
    chan = gr.channel_model(npwr)
    rrc = gr.fft_filter_ccc(1, rrc_taps)

    # Split it up into pieces
    channelizer = blks2.pfb_channelizer_ccf(nchans, proto_taps, 2)

    # Put the pieces back together again
    syn_taps = [nchans*t for t in proto_taps]
    synthesizer = gr.pfb_synthesizer_ccf(nchans, syn_taps, True)
    src_snk = gr.vector_sink_c()
    snk = gr.vector_sink_c()

    # Remap the location of the channels
    # Can be done in synth or channelizer (watch out for rotattions in
    # the channelizer)
    synthesizer.set_channel_map([ 0,  1,  2,  3,  4,
                                 15, 16, 17, 18, 19])

    tb = gr.top_block()
    tb.connect(src, mod, chan, rrc, channelizer)
    tb.connect(rrc, src_snk)

    vsnk = []
    for i in xrange(nchans):
        tb.connect((channelizer,i), (synthesizer, i))

        vsnk.append(gr.vector_sink_c())
        tb.connect((channelizer,i), vsnk[i])

    tb.connect(synthesizer, snk)
    tb.run()

    sin  = scipy.array(src_snk.data()[1000:])
    sout = scipy.array(snk.data()[1000:])


    # Plot original signal
    fs_in = nchans*fs
    f1 = pylab.figure(1, figsize=(16,12), facecolor='w')
    s11 = f1.add_subplot(2,2,1)
    s11.psd(sin, NFFT=fftlen, Fs=fs_in)
    s11.set_title("PSD of Original Signal")
    s11.set_ylim([-200, -20])

    s12 = f1.add_subplot(2,2,2)
    s12.plot(sin.real[1000:1500], "o-b")
    s12.plot(sin.imag[1000:1500], "o-r")
    s12.set_title("Original Signal in Time")

    start = 1
    skip  = 4
    s13 = f1.add_subplot(2,2,3)
    s13.plot(sin.real[start::skip], sin.imag[start::skip], "o")
    s13.set_title("Constellation")
    s13.set_xlim([-2, 2])
    s13.set_ylim([-2, 2])

    # Plot channels
    nrows = int(scipy.sqrt(nchans))
    ncols = int(scipy.ceil(float(nchans)/float(nrows)))

    f2 = pylab.figure(2, figsize=(16,12), facecolor='w')
    for n in xrange(nchans):
        s = f2.add_subplot(nrows, ncols, n+1)
        s.psd(vsnk[n].data(), NFFT=fftlen, Fs=fs_in)
        s.set_title("Channel {0}".format(n))
        s.set_ylim([-200, -20])

    # Plot reconstructed signal
    fs_out = 2*nchans*fs
    f3 = pylab.figure(3, figsize=(16,12), facecolor='w')
    s31 = f3.add_subplot(2,2,1)
    s31.psd(sout, NFFT=fftlen, Fs=fs_out)
    s31.set_title("PSD of Reconstructed Signal")
    s31.set_ylim([-200, -20])

    s32 = f3.add_subplot(2,2,2)
    s32.plot(sout.real[1000:1500], "o-b")
    s32.plot(sout.imag[1000:1500], "o-r")
    s32.set_title("Reconstructed Signal in Time")

    start = 2
    skip  = 4
    s33 = f3.add_subplot(2,2,3)
    s33.plot(sout.real[start::skip], sout.imag[start::skip], "o")
    s33.set_title("Constellation")
    s33.set_xlim([-2, 2])
    s33.set_ylim([-2, 2])

    pylab.show()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass


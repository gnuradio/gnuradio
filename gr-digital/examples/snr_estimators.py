#!/usr/bin/env python

import sys

try:
    import scipy
    from scipy import stats
except ImportError:
    print "Error: Program requires scipy (www.scipy.org)."
    sys.exit(1)

try:
    import pylab
except ImportError:
    print "Error: Program requires Matplotlib (matplotlib.sourceforge.net)."
    sys.exit(1)
    
from gnuradio import gr, digital
from optparse import OptionParser
from gnuradio.eng_option import eng_option

'''
This example program uses Python and GNU Radio to calculate SNR of a
noise BPSK signal to compare them.

For an explination of the online algorithms, see:
http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Higher-order_statistics
'''

def online_skewness(data, alpha):
    n = 0
    mean = 0
    M2 = 0
    M3 = 0
    d_M3 = 0

    for n in xrange(len(data)):
        delta = data[n] - mean
        delta_n = delta / (n+1)
        term1 = delta * delta_n * (n)
        mean = mean + delta_n
        M3 = term1 * delta_n * (n - 1) - 3 * delta_n * M2
        M2 = M2 + term1
        d_M3 = (0.001)*M3 + (1-0.001)*d_M3;
        
    return d_M3

def snr_est_simple(signal):
    y1 = scipy.mean(abs(signal))
    y2 = scipy.real(scipy.mean(signal**2))
    y3 = (y1*y1 - y2)
    snr_rat = y1*y1/y3
    return 10.0*scipy.log10(snr_rat), snr_rat
    
def snr_est_skew(signal):
    y1 = scipy.mean(abs(signal))
    y2 = scipy.mean(scipy.real(signal**2))
    y3 = (y1*y1 - y2)
    y4 = online_skewness(abs(signal.real), 0.001)

    skw = y4*y4 / (y2*y2*y2);
    snr_rat = y1*y1 / (y3 + skw*y1*y1)
    return 10.0*scipy.log10(snr_rat), snr_rat

def snr_est_m2m4(signal):
    M2 = scipy.mean(abs(signal)**2)
    M4 = scipy.mean(abs(signal)**4)
    snr_rat = 2*scipy.sqrt(2*M2*M2 - M4) / (M2 - scipy.sqrt(2*M2*M2 - M4))
    return 10.0*scipy.log10(snr_rat), snr_rat

def snr_est_svr(signal):
    N = len(signal)
    ssum = 0
    msum = 0
    for i in xrange(1, N):
        ssum += (abs(signal[i])**2)*(abs(signal[i-1])**2)
        msum += (abs(signal[i])**4)
    savg = (1.0/(float(N)-1.0))*ssum
    mavg = (1.0/(float(N)-1.0))*msum
    beta = savg / (mavg - savg)
    
    snr_rat = 2*((beta - 1) + scipy.sqrt(beta*(beta-1)))
    return 10.0*scipy.log10(snr_rat), snr_rat
    

def main():
    gr_estimators = {"simple": digital.SNR_EST_SIMPLE,
                     "skew": digital.SNR_EST_SKEW,
                     "m2m4": digital.SNR_EST_M2M4,
                     "svr": digital.SNR_EST_SVR}
    py_estimators = {"simple": snr_est_simple,
                     "skew": snr_est_skew,
                     "m2m4": snr_est_m2m4,
                     "svr": snr_est_svr}
    
    
    parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
    parser.add_option("-N", "--nsamples", type="int", default=10000,
                      help="Set the number of samples to process [default=%default]")
    parser.add_option("", "--snr-min", type="float", default=-5,
                      help="Minimum SNR [default=%default]")
    parser.add_option("", "--snr-max", type="float", default=20,
                      help="Maximum SNR [default=%default]")
    parser.add_option("", "--snr-step", type="float", default=0.5,
                      help="SNR step amount [default=%default]")
    parser.add_option("-t", "--type", type="choice",
                      choices=gr_estimators.keys(), default="simple",
                      help="Estimator type {0} [default=%default]".format(
                            gr_estimators.keys()))
    (options, args) = parser.parse_args ()

    N = options.nsamples
    xx = scipy.random.randn(N)
    xy = scipy.random.randn(N)
    bits = 2*scipy.complex64(scipy.random.randint(0, 2, N)) - 1

    snr_known = list()
    snr_python = list()
    snr_gr = list()
    
    # when to issue an SNR tag; can be ignored in this example.
    ntag = 10000

    n_cpx = xx + 1j*xy

    py_est = py_estimators[options.type]
    gr_est = gr_estimators[options.type]

    SNR_min = options.snr_min
    SNR_max = options.snr_max
    SNR_step = options.snr_step
    SNR_dB = scipy.arange(SNR_min, SNR_max+SNR_step, SNR_step)
    for snr in SNR_dB:
        SNR = 10.0**(snr/10.0)
        scale = scipy.sqrt(SNR)
        yy = bits + n_cpx/scale
        print "SNR: ", snr

        Sknown = scipy.mean(yy**2)
        Nknown = scipy.var(n_cpx/scale)/2
        snr0 = Sknown/Nknown
        snr0dB = 10.0*scipy.log10(snr0)
        snr_known.append(snr0dB)

        snrdB, snr = py_est(yy)        
        snr_python.append(snrdB)

        gr_src = gr.vector_source_c(bits.tolist(), False)
        gr_snr = digital.mpsk_snr_est_cc(gr_est, ntag, 0.001)
        gr_chn = gr.channel_model(1.0/scale)
        gr_snk = gr.null_sink(gr.sizeof_gr_complex)
        tb = gr.top_block()
        tb.connect(gr_src, gr_chn, gr_snr, gr_snk)
        tb.run()

        snr_gr.append(gr_snr.snr())

    f1 = pylab.figure(1)
    s1 = f1.add_subplot(1,1,1)
    s1.plot(SNR_dB, snr_known, "k-o", linewidth=2, label="Known")
    s1.plot(SNR_dB, snr_python, "b-o", linewidth=2, label="Python")
    s1.plot(SNR_dB, snr_gr, "g-o", linewidth=2, label="GNU Radio")
    s1.grid(True)
    s1.set_title('SNR Estimators')
    s1.set_xlabel('SNR (dB)')
    s1.set_ylabel('Estimated SNR')
    s1.legend()

    pylab.show()


if __name__ == "__main__":
    main()
    

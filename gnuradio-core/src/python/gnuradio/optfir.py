#
# Copyright 2004,2005,2009 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

'''
Routines for designing optimal FIR filters.

For a great intro to how all this stuff works, see section 6.6 of
"Digital Signal Processing: A Practical Approach", Emmanuael C. Ifeachor
and Barrie W. Jervis, Adison-Wesley, 1993.  ISBN 0-201-54413-X.
'''

import math, cmath
from gnuradio import gr

remez = gr.remez

# ----------------------------------------------------------------

##  Builds a low pass filter.
#   @param gain  Filter gain in the passband (linear)
#   @param Fs    Sampling rate (sps)
#   @param freq1 End of pass band (in Hz)
#   @param freq2 Start of stop band (in Hz)
#   @param passband_ripple_db Pass band ripple in dB (should be small, < 1)
#   @param stopband_atten_db  Stop band attenuation in dB (should be large, >= 60)
#   @param nextra_taps  Extra taps to use in the filter (default=2)
def low_pass (gain, Fs, freq1, freq2, passband_ripple_db, stopband_atten_db,
              nextra_taps=2):
    passband_dev = passband_ripple_to_dev (passband_ripple_db)
    stopband_dev = stopband_atten_to_dev (stopband_atten_db)
    desired_ampls = (gain, 0)
    (n, fo, ao, w) = remezord ([freq1, freq2], desired_ampls,
                               [passband_dev, stopband_dev], Fs)
    # The remezord typically under-estimates the filter order, so add 2 taps by default
    taps = gr.remez (n + nextra_taps, fo, ao, w, "bandpass")
    return taps

##  Builds a band pass filter.
#   @param gain  Filter gain in the passband (linear)
#   @param Fs    Sampling rate (sps)
#   @param freq_sb1 End of stop band (in Hz)
#   @param freq_pb1 Start of pass band (in Hz)
#   @param freq_pb2 End of pass band (in Hz)
#   @param freq_sb2 Start of stop band (in Hz)
#   @param passband_ripple_db Pass band ripple in dB (should be small, < 1)
#   @param stopband_atten_db  Stop band attenuation in dB (should be large, >= 60)
#   @param nextra_taps  Extra taps to use in the filter (default=2)
def band_pass (gain, Fs, freq_sb1, freq_pb1, freq_pb2, freq_sb2,
               passband_ripple_db, stopband_atten_db,
               nextra_taps=2):
    passband_dev = passband_ripple_to_dev (passband_ripple_db)
    stopband_dev = stopband_atten_to_dev (stopband_atten_db)
    desired_ampls = (0, gain, 0)
    desired_freqs = [freq_sb1, freq_pb1, freq_pb2, freq_sb2]
    desired_ripple = [stopband_dev, passband_dev, stopband_dev]
    (n, fo, ao, w) = remezord (desired_freqs, desired_ampls,
                               desired_ripple, Fs)
    # The remezord typically under-estimates the filter order, so add 2 taps by default
    taps = gr.remez (n + nextra_taps, fo, ao, w, "bandpass")
    return taps


##  Builds a band pass filter with complex taps by making an LPF and
#   spinning it up to the right center frequency
#   @param gain  Filter gain in the passband (linear)
#   @param Fs    Sampling rate (sps)
#   @param freq_sb1 End of stop band (in Hz)
#   @param freq_pb1 Start of pass band (in Hz)
#   @param freq_pb2 End of pass band (in Hz)
#   @param freq_sb2 Start of stop band (in Hz)
#   @param passband_ripple_db Pass band ripple in dB (should be small, < 1)
#   @param stopband_atten_db  Stop band attenuation in dB (should be large, >= 60)
#   @param nextra_taps  Extra taps to use in the filter (default=2)
def complex_band_pass (gain, Fs, freq_sb1, freq_pb1, freq_pb2, freq_sb2,
                       passband_ripple_db, stopband_atten_db,
                       nextra_taps=2):
    center_freq = (freq_pb2 + freq_pb1) / 2.0
    lp_pb = (freq_pb2 - center_freq)/1.0
    lp_sb = freq_sb2 - center_freq
    lptaps = low_pass(gain, Fs, lp_pb, lp_sb, passband_ripple_db,
                      stopband_atten_db, nextra_taps)
    spinner = [cmath.exp(2j*cmath.pi*center_freq/Fs*i) for i in xrange(len(lptaps))]
    taps = [s*t for s,t in zip(spinner, lptaps)]
    return taps


##  Builds a band reject filter
#   spinning it up to the right center frequency
#   @param gain  Filter gain in the passband (linear)
#   @param Fs    Sampling rate (sps)
#   @param freq_pb1 End of pass band (in Hz)
#   @param freq_sb1 Start of stop band (in Hz)
#   @param freq_sb2 End of stop band (in Hz)
#   @param freq_pb2 Start of pass band (in Hz)
#   @param passband_ripple_db Pass band ripple in dB (should be small, < 1)
#   @param stopband_atten_db  Stop band attenuation in dB (should be large, >= 60)
#   @param nextra_taps  Extra taps to use in the filter (default=2)
def band_reject (gain, Fs, freq_pb1, freq_sb1, freq_sb2, freq_pb2,
                 passband_ripple_db, stopband_atten_db,
                 nextra_taps=2):
    passband_dev = passband_ripple_to_dev (passband_ripple_db)
    stopband_dev = stopband_atten_to_dev (stopband_atten_db)
    desired_ampls = (gain, 0, gain)
    desired_freqs = [freq_pb1, freq_sb1, freq_sb2, freq_pb2]
    desired_ripple = [passband_dev, stopband_dev, passband_dev]
    (n, fo, ao, w) = remezord (desired_freqs, desired_ampls,
                               desired_ripple, Fs)
    # Make sure we use an odd number of taps
    if((n+nextra_taps)%2 == 1):
        n += 1
    # The remezord typically under-estimates the filter order, so add 2 taps by default
    taps = gr.remez (n + nextra_taps, fo, ao, w, "bandpass")
    return taps


##  Builds a high pass filter.
#   @param gain  Filter gain in the passband (linear)
#   @param Fs    Sampling rate (sps)
#   @param freq1 End of stop band (in Hz)
#   @param freq2 Start of pass band (in Hz)
#   @param passband_ripple_db Pass band ripple in dB (should be small, < 1)
#   @param stopband_atten_db  Stop band attenuation in dB (should be large, >= 60)
#   @param nextra_taps  Extra taps to use in the filter (default=2)
def high_pass (gain, Fs, freq1, freq2, passband_ripple_db, stopband_atten_db,
               nextra_taps=2):
    passband_dev = passband_ripple_to_dev (passband_ripple_db)
    stopband_dev = stopband_atten_to_dev (stopband_atten_db)
    desired_ampls = (0, 1)
    (n, fo, ao, w) = remezord ([freq1, freq2], desired_ampls,
                               [stopband_dev, passband_dev], Fs)
    # For a HPF, we need to use an odd number of taps
    # In gr.remez, ntaps = n+1, so n must be even
    if((n+nextra_taps)%2 == 1):
        n += 1
        
    # The remezord typically under-estimates the filter order, so add 2 taps by default
    taps = gr.remez (n + nextra_taps, fo, ao, w, "bandpass")
    return taps

# ----------------------------------------------------------------

def stopband_atten_to_dev (atten_db):
    """Convert a stopband attenuation in dB to an absolute value"""
    return 10**(-atten_db/20)

def passband_ripple_to_dev (ripple_db):
    """Convert passband ripple spec expressed in dB to an absolute value"""
    return (10**(ripple_db/20)-1)/(10**(ripple_db/20)+1)

# ----------------------------------------------------------------

def remezord (fcuts, mags, devs, fsamp = 2):
    '''
    FIR order estimator (lowpass, highpass, bandpass, mulitiband).

    (n, fo, ao, w) = remezord (f, a, dev)
    (n, fo, ao, w) = remezord (f, a, dev, fs)

    (n, fo, ao, w) = remezord (f, a, dev) finds the approximate order,
    normalized frequency band edges, frequency band amplitudes, and
    weights that meet input specifications f, a, and dev, to use with
    the remez command.

    * f is a sequence of frequency band edges (between 0 and Fs/2, where
      Fs is the sampling frequency), and a is a sequence specifying the
      desired amplitude on the bands defined by f. The length of f is
      twice the length of a, minus 2. The desired function is
      piecewise constant.

    * dev is a sequence the same size as a that specifies the maximum
      allowable deviation or ripples between the frequency response
      and the desired amplitude of the output filter, for each band.

    Use remez with the resulting order n, frequency sequence fo,
    amplitude response sequence ao, and weights w to design the filter b
    which approximately meets the specifications given by remezord
    input parameters f, a, and dev:

    b = remez (n, fo, ao, w)

    (n, fo, ao, w) = remezord (f, a, dev, Fs) specifies a sampling frequency Fs.

    Fs defaults to 2 Hz, implying a Nyquist frequency of 1 Hz. You can
    therefore specify band edges scaled to a particular applications
    sampling frequency.

    In some cases remezord underestimates the order n. If the filter
    does not meet the specifications, try a higher order such as n+1
    or n+2.
    '''
    # get local copies
    fcuts = fcuts[:]
    mags = mags[:]
    devs = devs[:]
    
    for i in range (len (fcuts)):
        fcuts[i] = float (fcuts[i]) / fsamp

    nf = len (fcuts)
    nm = len (mags)
    nd = len (devs)
    nbands = nm

    if nm != nd:
        raise ValueError, "Length of mags and devs must be equal"

    if nf != 2 * (nbands - 1):
        raise ValueError, "Length of f must be 2 * len (mags) - 2"
    
    for i in range (len (mags)):
        if mags[i] != 0:                        # if not stopband, get relative deviation
            devs[i] = devs[i] / mags[i]
        
    # separate the passband and stopband edges
    f1 = fcuts[0::2]
    f2 = fcuts[1::2]

    n = 0
    min_delta = 2
    for i in range (len (f1)):
        if f2[i] - f1[i] < min_delta:
            n = i
            min_delta = f2[i] - f1[i]

    if nbands == 2:
        # lowpass or highpass case (use formula)
        l = lporder (f1[n], f2[n], devs[0], devs[1])
    else:
        # bandpass or multipass case
        # try different lowpasses and take the worst one that
        #  goes through the BP specs
        l = 0
        for i in range (1, nbands-1):
            l1 = lporder (f1[i-1], f2[i-1], devs[i], devs[i-1])
            l2 = lporder (f1[i], f2[i], devs[i], devs[i+1])
            l = max (l, l1, l2)

    n = int (math.ceil (l)) - 1               # need order, not length for remez
    
    # cook up remez compatible result
    ff = [0] + fcuts + [1]
    for i in range (1, len (ff) - 1):
        ff[i] *= 2

    aa = []
    for a in mags:
        aa = aa + [a, a]

    max_dev = max (devs)
    wts = [1] * len(devs)
    for i in range (len (wts)):
        wts[i] = max_dev / devs[i]

    return (n, ff, aa, wts)

# ----------------------------------------------------------------

def lporder (freq1, freq2, delta_p, delta_s):
    '''
    FIR lowpass filter length estimator.  freq1 and freq2 are
    normalized to the sampling frequency.  delta_p is the passband
    deviation (ripple), delta_s is the stopband deviation (ripple).

    Note, this works for high pass filters too (freq1 > freq2), but
    doesnt work well if the transition is near f == 0 or f == fs/2

    From Herrmann et al (1973), Practical design rules for optimum
    finite impulse response filters.  Bell System Technical J., 52, 769-99
    '''
    df = abs (freq2 - freq1)
    ddp = math.log10 (delta_p)
    dds = math.log10 (delta_s)

    a1 = 5.309e-3
    a2 = 7.114e-2
    a3 = -4.761e-1
    a4 = -2.66e-3
    a5 = -5.941e-1
    a6 = -4.278e-1

    b1 = 11.01217
    b2 = 0.5124401

    t1 = a1 * ddp * ddp
    t2 = a2 * ddp
    t3 = a4 * ddp * ddp
    t4 = a5 * ddp

    dinf=((t1 + t2 + a3) * dds) + (t3 + t4 + a6)
    ff = b1 + b2 * (ddp - dds)
    n = dinf / df - ff * df + 1
    return n


def bporder (freq1, freq2, delta_p, delta_s):
    '''
    FIR bandpass filter length estimator.  freq1 and freq2 are
    normalized to the sampling frequency.  delta_p is the passband
    deviation (ripple), delta_s is the stopband deviation (ripple).

    From Mintzer and Liu (1979)
    '''
    df = abs (freq2 - freq1)
    ddp = math.log10 (delta_p)
    dds = math.log10 (delta_s)

    a1 = 0.01201
    a2 = 0.09664
    a3 = -0.51325
    a4 = 0.00203
    a5 = -0.57054
    a6 = -0.44314

    t1 = a1 * ddp * ddp
    t2 = a2 * ddp
    t3 = a4 * ddp * ddp
    t4 = a5 * ddp

    cinf = dds * (t1 + t2 + a3) + t3 + t4 + a6
    ginf = -14.6 * math.log10 (delta_p / delta_s) - 16.9
    n = cinf / df + ginf * df + 1
    return n


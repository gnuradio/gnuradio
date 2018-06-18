#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
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

import re
import math
import sys
import operator
import numpy

#from gnuradio import trellis

try:
    import scipy.linalg
except ImportError:
    print "Error: Program requires scipy (see: www.scipy.org)."
    sys.exit(1)


def dec2base(num, base, l):
    """
    Decimal to any base conversion.
    Convert 'num' to a list of 'l' numbers representing 'num'
    to base 'base' (most significant symbol first).
    """
    s = range(l)
    n = num
    for i in range(l):
        s[l - i - 1] = n % base
        n = int(n / base)
    if n != 0:
        print 'Number ', num, ' requires more than ', l, 'digits.'
    return s


def base2dec(s, base):
    """
    Conversion from any base to decimal.
    Convert a list 's' of symbols to a decimal number
    (most significant symbol first)
    """
    num = 0
    for i in range(len(s)):
        num = num * base + s[i]
    return num


def make_isi_lookup(mod, channel, normalize):
    """
    Automatically generate the lookup table that maps the FSM outputs
    to channel inputs corresponding to a channel 'channel' and a modulation
    'mod'. Optional normalization of channel to unit energy.
    This table is used by the 'metrics' block to translate
    channel outputs to metrics for use with the Viterbi algorithm.
    Limitations: currently supports only one-dimensional modulations.
    """
    dim = mod[0]
    constellation = mod[1]

    if normalize:
        p = 0
        for i in range(len(channel)):
            p = p + channel[i]**2
        for i in range(len(channel)):
            channel[i] = channel[i] / math.sqrt(p)

    lookup = range(len(constellation)**len(channel))
    for o in range(len(constellation)**len(channel)):
        ss = dec2base(o, len(constellation), len(channel))
        ll = 0
        for i in range(len(channel)):
            ll = ll + constellation[ss[i]] * channel[i]
            lookup[o] = ll
    return (1, lookup)


def make_cpm_signals(K, P, M, L, q, frac):
    """
    Automatically generate the signals appropriate for CPM
    decomposition.
    This decomposition is based on the paper by B. Rimoldi
    "A decomposition approach to CPM", IEEE Trans. Info Theory, March 1988
    See also my own notes at http://www.eecs.umich.edu/~anastas/docs/cpm.pdf
    """
    Q = numpy.size(q) / L
    h = (1.0 * K) / P
    f0 = -h * (M - 1) / 2
    dt = 0.0
    # maybe start at t=0.5
    t = (dt + numpy.arange(0, Q)) / Q
    qq = numpy.zeros(Q)
    for m in range(L):
        qq = qq + q[m * Q:m * Q + Q]
        w = math.pi * h * (M - 1) * t - 2 * math.pi * h * (
            M - 1) * qq + math.pi * h * (L - 1) * (M - 1)

    X = (M**L) * P
    PSI = numpy.empty((X, Q))
    for x in range(X):
        xv = dec2base(x / P, M, L)
        xv = numpy.append(xv, x % P)
        qq1 = numpy.zeros(Q)
        for m in range(L):
            qq1 = qq1 + xv[m] * q[m * Q:m * Q + Q]
            psi = 2 * math.pi * h * xv[-1] + 4 * math.pi * h * qq1 + w
        PSI[x] = psi
        PSI = numpy.transpose(PSI)
        SS = numpy.exp(1j * PSI)  # contains all signals as columns

    # Now we need to orthogonalize the signals
    F = scipy.linalg.orth(SS)  # find an orthonormal basis for SS
    #print numpy.dot(numpy.transpose(F.conjugate()),F) # check for orthonormality
    S = numpy.dot(numpy.transpose(F.conjugate()), SS)
    #print F
    #print S

    # We only want to keep those dimensions that contain most
    # of the energy of the overall constellation (eg, frac=0.9 ==> 90%)
    # evaluate mean energy in each dimension
    E = numpy.sum(numpy.absolute(S)**2, axis=1) / Q
    E = E / numpy.sum(E)
    #print E
    Es = -numpy.sort(-E)
    Esi = numpy.argsort(-E)
    #print Es
    #print Esi
    Ecum = numpy.cumsum(Es)
    #print Ecum
    v0 = numpy.searchsorted(Ecum, frac)
    N = v0 + 1
    #print v0
    #print Esi[0:v0+1]
    Ff = numpy.transpose(numpy.transpose(F)[Esi[0:v0 + 1]])
    #print Ff
    Sf = S[Esi[0:v0 + 1]]
    #print Sf

    return (f0, SS, S, F, Sf, Ff, N)


#return f0

######################################################################
# A list of common modulations.
# Format: (dimensionality,constellation)
######################################################################
pam2 = (1, [-1, 1])
pam4 = (1, [-3, -1, 3, 1])  # includes Gray mapping
pam8 = (1, [-7, -5, -3, -1, 1, 3, 5, 7])

psk4=(2,[1, 0, \
         0, 1, \
         0, -1,\
         -1, 0])    # includes Gray mapping

psk8=(2,[math.cos(2*math.pi*0/8), math.sin(2*math.pi*0/8),  \
         math.cos(2*math.pi*1/8), math.sin(2*math.pi*1/8),  \
         math.cos(2*math.pi*2/8), math.sin(2*math.pi*2/8),  \
         math.cos(2*math.pi*3/8), math.sin(2*math.pi*3/8),  \
         math.cos(2*math.pi*4/8), math.sin(2*math.pi*4/8),  \
         math.cos(2*math.pi*5/8), math.sin(2*math.pi*5/8),  \
         math.cos(2*math.pi*6/8), math.sin(2*math.pi*6/8),  \
         math.cos(2*math.pi*7/8), math.sin(2*math.pi*7/8)])

psk2x3 = (3,[-1,-1,-1, \
             -1,-1,1, \
             -1,1,-1, \
             -1,1,1, \
             1,-1,-1, \
             1,-1,1, \
             1,1,-1, \
             1,1,1])

psk2x4 = (4,[-1,-1,-1,-1, \
             -1,-1,-1,1,  \
             -1,-1,1,-1,  \
             -1,-1,1,1,   \
             -1,1,-1,-1,  \
             -1,1,-1,1,   \
             -1,1,1,-1,   \
             -1,1,1,1,    \
             1,-1,-1,-1,  \
             1,-1,-1,1,   \
             1,-1,1,-1,   \
             1,-1,1,1,    \
             1,1,-1,-1,   \
             1,1,-1,1,    \
             1,1,1,-1,    \
             1,1,1,1])

orth2 = (2,[1, 0, \
            0, 1])
orth4=(4,[1, 0, 0, 0, \
          0, 1, 0, 0, \
          0, 0, 1, 0, \
          0, 0, 0, 1])

######################################################################
# A list of channels to be tested
######################################################################

# C test channel (J. Proakis, Digital Communications, McGraw-Hill Inc., 2001)
c_channel = [0.227, 0.460, 0.688, 0.460, 0.227]

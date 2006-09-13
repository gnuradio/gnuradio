#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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




######################################################################
# Decimal to any base conversion.
# Convert 'num' to a list of 'l' numbers representing 'num'
# to base 'base' (most significant symbol first).
######################################################################
def dec2base(num,base,l):
    s=range(l)
    n=num
    for i in range(l):
        s[l-i-1]=n%base
        n=int(n/base)
    if n!=0:
        print 'Number ', num, ' requires more than ', l, 'digits.'
    return s


######################################################################
# Conversion from any base to decimal.
# Convert a list 's' of symbols to a decimal number
# (most significant symbol first)
######################################################################
def base2dec(s,base):
    num=0
    for i in range(len(s)):
        num=num*base+s[i]
    return num





######################################################################
# Automatically generate the lookup table that maps the FSM outputs
# to channel inputs corresponding to a channel 'channel' and a modulation
# 'mod'. Optional normalization of channel to unit energy.
# This table is used by the 'metrics' block to translate
# channel outputs to metrics for use with the Viterbi algorithm. 
# Limitations: currently supports only one-dimensional modulations.
######################################################################
def make_isi_lookup(mod,channel,normalize):
    dim=mod[0]
    constellation = mod[1]

    if normalize:
        p = 0
        for i in range(len(channel)):
            p = p + channel[i]**2
        for i in range(len(channel)):
            channel[i] = channel[i]/math.sqrt(p)

    lookup=range(len(constellation)**len(channel))
    for o in range(len(constellation)**len(channel)):
        ss=dec2base(o,len(constellation),len(channel))
        ll=0
        for i in range(len(channel)):
            ll=ll+constellation[ss[i]]*channel[i]
        lookup[o]=ll
    return (1,lookup)


    



######################################################################
# A list of common modulations.
# Format: (dimensionality,constellation)
######################################################################
pam2 = (1,[-1, 1])
pam4 = (1,[-3, -1, 3, 1])		# includes Gray mapping
pam8 = (1,[-7, -5, -3, -1, 1, 3, 5, 7])

psk4=(2,[1, 0, \
         0, 1, \
         0, -1,\
        -1, 0])				# includes Gray mapping
psk8=(2,[math.cos(2*math.pi*0/8), math.sin(2*math.pi*0/8),  \
         math.cos(2*math.pi*1/8), math.sin(2*math.pi*1/8),  \
         math.cos(2*math.pi*2/8), math.sin(2*math.pi*2/8),  \
         math.cos(2*math.pi*3/8), math.sin(2*math.pi*3/8),  \
         math.cos(2*math.pi*4/8), math.sin(2*math.pi*4/8),  \
         math.cos(2*math.pi*5/8), math.sin(2*math.pi*5/8),  \
         math.cos(2*math.pi*6/8), math.sin(2*math.pi*6/8),  \
         math.cos(2*math.pi*7/8), math.sin(2*math.pi*7/8)])

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










if __name__ == '__main__':
    make_fsm_bin_cc_ff (1,2,[[7,5]])
    print "----------\n"
    make_fsm_bin_cc_ff (2,3,[[1,0,2],[0,1,6]])


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

from gnuradio import trellis



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
# Generate a new FSM representing the concatenation of two FSMs
######################################################################
def fsm_concatenate(f1,f2):
    if f1.O() > f2.I():
        print "Not compatible FSMs\n"
    I=f1.I()
    S=f1.S()*f2.S() 
    O=f2.O()
    nsm=list([0]*I*S)
    osm=list([0]*I*S)
    for s1 in range(f1.S()):
        for s2 in range(f2.S()):
            for i in range(f1.I()):
                ns1=f1.NS()[s1*f1.I()+i]
                o1=f1.OS()[s1*f1.I()+i]
                ns2=f2.NS()[s2*f2.I()+o1]
                o2=f2.OS()[s2*f2.I()+o1]

                s=s1*f2.S()+s2
                ns=ns1*f2.S()+ns2
                nsm[s*I+i]=ns
                osm[s*I+i]=o2


    f=trellis.fsm(I,S,O,nsm,osm)
    return f

######################################################################
# Generate a new FSM representing n stages through the original FSM
######################################################################
def fsm_radix(f,n):
    I=f.I()**n
    S=f.S()
    O=f.O()**n
    nsm=list([0]*I*S)
    osm=list([0]*I*S)
    for s in range(f.S()):
        for i in range(I):
            ii=dec2base(i,f.I(),n) 
            oo=list([0]*n)
            ns=s
            for k in range(n):
                oo[k]=f.OS()[ns*f.I()+ii[k]]
                ns=f.NS()[ns*f.I()+ii[k]]

            nsm[s*I+i]=ns
            osm[s*I+i]=base2dec(oo,f.O())


    f=trellis.fsm(I,S,O,nsm,osm)
    return f




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
    f1=trellis.fsm('fsm_files/awgn1o2_4.fsm')
    #f2=trellis.fsm('fsm_files/awgn2o3_4.fsm')
    print f1.I(), f1.S(), f1.O()
    print f1.NS()
    print f1.OS()
    #print f2.I(), f2.S(), f2.O()
    #print f2.NS()
    #print f2.OS()
    ##f1.write_trellis_svg('f1.svg',4)
    #f2.write_trellis_svg('f2.svg',4)
    #f=fsm_concatenate(f1,f2)
    f=fsm_radix(f1,2)

    print "----------\n"
    print f.I(), f.S(), f.O()
    print f.NS()
    print f.OS()
    #f.write_trellis_svg('f.svg',4)


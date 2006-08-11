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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
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
# Automaticaly generate the FSM structure for a binary feed-forward
# convolutional code.
# Input: k x n generator matrix (decimal representation)
######################################################################
def make_fsm_bin_cc_ff(k,n,GM):
    mem=[[]]*k
    max_mem_x=[-1]*k
    max_mem = -1
    for i in range(k):
        memr=[0]*n
        for j in range(n):
            if GM[i][j]==0:
                memr[j]=-1
            else:
                memr[j]=int(math.log(GM[i][j],2))
            if memr[j]>max_mem_x[i]:
                max_mem_x[i]=memr[j]
            if memr[j]>max_mem:
                max_mem=memr[j]
        mem[i]=memr

    sum_max_mem = 0
    for i in range(k):
       sum_max_mem = sum_max_mem+max_mem_x[i] 
        

    #print mem
    #print max_mem_x
    #print max_mem
    #print sum_max_mem

    I=2**k
    S=2**sum_max_mem
    O=2**n

    #print I, S, O

    NS=[0]*S*I;
    OS=[0]*S*I;
    for s in range(S):
        for i in range(I):
            ss=dec2base(s,2,sum_max_mem)
            ind=0
            ss_r=[]
            for kk in range(k):
                ss1 = [0]*max_mem
                ss1[0:max_mem_x[kk]] = ss[ind:ind+max_mem_x[kk]]
                ss_r.append(ss1)
                ind=ind+max_mem_x[kk]
            ii=dec2base(i,2,k)

            tt_r = ss_r
            for kk in range(k):
                tt_r[kk].insert(0,ii[kk])
            #print tt_r

            ns_r = []
            for kk in range(k):
                ns_r.append(tt_r[kk][0:max_mem])

            ns=[]
            for kk in range(k):
                ns = ns + ns_r[kk][0:max_mem_x[kk]]
            NS[s*I+i]=base2dec(ns,2);

            out_r=[0]*n
            for nn in range(n):
                out=0;
                for kk in range(k):
                    c=[0]*max_mem
                    gm = dec2base(GM[kk][nn],2,max_mem_x[kk]+1)
                    gm.reverse()
                    c[0:len(gm)] = gm
                    sy = 0
                    for m in range(len(c)):
                        sy = sy + c[m]*tt_r[kk][m];
                    out=operator.mod(out+sy,2);
                out_r[nn]=out;
            out_r.reverse()
            OS[s*I+i] = base2dec(out_r,2);
    
    #O=max(max(OS))+1;
    print I, S, O
    print NS
    print OS

    return (I,S,O,NS,OS)





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


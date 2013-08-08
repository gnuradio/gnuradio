#!/usr/bin/env python

from gnuradio import gr
from gnuradio import audio
from gnuradio import trellis, digital, filter, blocks
from gnuradio import eng_notation
import math
import sys
import random
import fsm_utils

try:
    from gnuradio import analog
except ImportError:
    sys.stderr.write("Error: Program requires gr-analog.\n")
    sys.exit(1)

def run_test (f,Kb,bitspersymbol,K,channel,modulation,dimensionality,tot_constellation,N0,seed):
    tb = gr.top_block ()
    L = len(channel)

    # TX
    # this for loop is TOO slow in python!!!
    packet = [0]*(K+2*L)
    random.seed(seed)
    for i in range(len(packet)):
        packet[i] = random.randint(0, 2**bitspersymbol - 1) # random symbols
    for i in range(L): # first/last L symbols set to 0
        packet[i] = 0
        packet[len(packet)-i-1] = 0
    src = blocks.vector_source_s(packet,False)
    mod = digital.chunks_to_symbols_sf(modulation[1],modulation[0])

    # CHANNEL
    isi = filter.fir_filter_fff(1,channel)
    add = blocks.add_ff()
    noise = analog.noise_source_f(analog.GR_GAUSSIAN,math.sqrt(N0/2),seed)

    # RX
    skip = blocks.skiphead(gr.sizeof_float, L) # skip the first L samples since you know they are coming from the L zero symbols
    #metrics = trellis.metrics_f(f.O(),dimensionality,tot_constellation,digital.TRELLIS_EUCLIDEAN) # data preprocessing to generate metrics for Viterbi
    #va = trellis.viterbi_s(f,K+L,0,0) # Put -1 if the Initial/Final states are not set.
    va = trellis.viterbi_combined_s(f,K+L,0,0,dimensionality,tot_constellation,digital.TRELLIS_EUCLIDEAN) # using viterbi_combined_s instead of metrics_f/viterbi_s allows larger packet lengths because metrics_f is complaining for not being able to allocate large buffers. This is due to the large f.O() in this application...
    dst = blocks.vector_sink_s()

    tb.connect (src,mod)
    tb.connect (mod,isi,(add,0))
    tb.connect (noise,(add,1))
    #tb.connect (add,metrics)
    #tb.connect (metrics,va,dst)
    tb.connect (add,skip,va,dst)

    tb.run()

    data = dst.data()
    ntotal = len(data) - L
    nright=0
    for i in range(ntotal):
        if packet[i+L]==data[i]:
            nright=nright+1
        #else:
            #print "Error in ", i

    return (ntotal,ntotal-nright)


def main(args):
    nargs = len (args)
    if nargs == 2:
        esn0_db=float(args[0])
        rep=int(args[1])
    else:
        sys.stderr.write ('usage: test_viterbi_equalization1.py Es/No_db  repetitions\n')
        sys.exit (1)

    # system parameters
    Kb=2048  # packet size in bits
    modulation = fsm_utils.pam4 # see fsm_utlis.py for available predefined modulations
    channel = fsm_utils.c_channel # see fsm_utlis.py for available predefined test channels
    f=trellis.fsm(len(modulation[1]),len(channel)) # generate the FSM automatically
    bitspersymbol = int(round(math.log(f.I())/math.log(2))) # bits per FSM input symbol
    K=Kb/bitspersymbol # packet size in trellis steps

    tot_channel = fsm_utils.make_isi_lookup(modulation,channel,True) # generate the lookup table (normalize energy to 1)
    dimensionality = tot_channel[0]
    tot_constellation = tot_channel[1]
    N0=pow(10.0,-esn0_db/10.0); # noise variance
    if len(tot_constellation)/dimensionality != f.O():
        sys.stderr.write ('Incompatible FSM output cardinality and lookup table size.\n')
        sys.exit (1)

    tot_s=0 # total number of transmitted shorts
    terr_s=0 # total number of shorts in error
    terr_p=0 # total number of packets in error

    for i in range(rep):
        (s,e)=run_test(f,Kb,bitspersymbol,K,channel,modulation,dimensionality,tot_constellation,N0,-long(666+i)) # run experiment with different seed to get different data and noise realizations
        tot_s=tot_s+s
        terr_s=terr_s+e
        terr_p=terr_p+(terr_s!=0)
        if ((i+1)%100==0) : # display progress
            print i+1,terr_p, '%.2e' % ((1.0*terr_p)/(i+1)),tot_s,terr_s, '%.2e' % ((1.0*terr_s)/tot_s)
    # estimate of the (short or symbol) error rate
    print rep,terr_p, '%.2e' % ((1.0*terr_p)/(i+1)),tot_s,terr_s, '%.2e' % ((1.0*terr_s)/tot_s)


if __name__ == '__main__':
    main (sys.argv[1:])

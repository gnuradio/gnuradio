#!/usr/bin/env python

from gnuradio import gr
from gnuradio import trellis, digital
from gnuradio import eng_notation
import math
import sys
import fsm_utils

def run_test (f,Kb,bitspersymbol,K,dimensionality,tot_constellation,N0,seed):
    tb = gr.top_block ()

    # TX
    src = gr.lfsr_32k_source_s()
    src_head = gr.head (gr.sizeof_short,Kb/16) # packet size in shorts
    s2fsmi = gr.packed_to_unpacked_ss(bitspersymbol,gr.GR_MSB_FIRST) # unpack shorts to symbols compatible with the FSM input cardinality
    enc = trellis.encoder_ss(f,0) # initial state = 0
    # essentially here we implement the combination of modulation and channel as a memoryless modulation (the memory induced by the channel is hidden in the FSM)
    mod = gr.chunks_to_symbols_sf(tot_constellation,dimensionality)

    # CHANNEL
    add = gr.add_ff()
    noise = gr.noise_source_f(gr.GR_GAUSSIAN,math.sqrt(N0/2),seed)
    
    # RX
    metrics = trellis.metrics_f(f.O(),dimensionality,tot_constellation,digital.TRELLIS_EUCLIDEAN) # data preprocessing to generate metrics for Viterbi
    va = trellis.viterbi_s(f,K,0,-1) # Put -1 if the Initial/Final states are not set.
    fsmi2s = gr.unpacked_to_packed_ss(bitspersymbol,gr.GR_MSB_FIRST) # pack FSM input symbols to shorts
    dst = gr.check_lfsr_32k_s(); 
    
    tb.connect (src,src_head,s2fsmi,enc,mod)
    tb.connect (mod,(add,0))
    tb.connect (noise,(add,1))
    tb.connect (add,metrics)
    tb.connect (metrics,va,fsmi2s,dst)
    
    tb.run()

    ntotal = dst.ntotal ()
    nright = dst.nright ()
    runlength = dst.runlength ()
    #print ntotal,nright,runlength 
    
    return (ntotal,ntotal-nright)




def main(args):
    nargs = len (args)
    if nargs == 2:
        esn0_db=float(args[0])
        rep=int(args[1])
    else:
        sys.stderr.write ('usage: test_viterbi_equalization.py Es/No_db  repetitions\n')
        sys.exit (1)

    # system parameters
    Kb=128*16  # packet size in bits (multiple of 16)
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
        (s,e)=run_test(f,Kb,bitspersymbol,K,dimensionality,tot_constellation,N0,-long(666+i)) # run experiment with different seed to get different noise realizations
        tot_s=tot_s+s
        terr_s=terr_s+e
        terr_p=terr_p+(terr_s!=0)
        if ((i+1)%100==0) : # display progress
            print i+1,terr_p, '%.2e' % ((1.0*terr_p)/(i+1)),tot_s,terr_s, '%.2e' % ((1.0*terr_s)/tot_s)
    # estimate of the (short or bit) error rate
    print rep,terr_p, '%.2e' % ((1.0*terr_p)/(i+1)),tot_s,terr_s, '%.2e' % ((1.0*terr_s)/tot_s)



if __name__ == '__main__':
    main (sys.argv[1:])


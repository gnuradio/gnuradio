#!/usr/bin/env python

from gnuradio import gr
from gnuradio import trellis, digital
from gnuradio import eng_notation
import math
import sys
import random
import fsm_utils




def run_test (fo,fi,interleaver,Kb,bitspersymbol,K,dimensionality,constellation,N0,seed):
    tb = gr.top_block ()


    # TX
    src = gr.lfsr_32k_source_s()
    src_head = gr.head (gr.sizeof_short,Kb/16) # packet size in shorts
    s2fsmi = gr.packed_to_unpacked_ss(bitspersymbol,gr.GR_MSB_FIRST) # unpack shorts to symbols compatible with the outer FSM input cardinality
    enc_out = trellis.encoder_ss(fo,0) # initial state = 0
    inter = trellis.permutation(interleaver.K(),interleaver.INTER(),1,gr.sizeof_short)
    enc_in = trellis.encoder_ss(fi,0) # initial state = 0
    mod = gr.chunks_to_symbols_sf(constellation,dimensionality)

    # CHANNEL
    add = gr.add_ff()
    noise = gr.noise_source_f(gr.GR_GAUSSIAN,math.sqrt(N0/2),seed)

    # RX
    metrics_in = trellis.metrics_f(fi.O(),dimensionality,constellation,digital.TRELLIS_EUCLIDEAN) # data preprocessing to generate metrics for innner Viterbi
    gnd = gr.vector_source_f([0],True);
    siso_in = trellis.siso_f(fi,K,0,-1,True,False,trellis.TRELLIS_MIN_SUM) # Put -1 if the Initial/Final states are not set.
    deinter = trellis.permutation(interleaver.K(),interleaver.DEINTER(),fi.I(),gr.sizeof_float)
    va_out = trellis.viterbi_s(fo,K,0,-1) # Put -1 if the Initial/Final states are not set.
    fsmi2s = gr.unpacked_to_packed_ss(bitspersymbol,gr.GR_MSB_FIRST) # pack FSM input symbols to shorts
    dst = gr.check_lfsr_32k_s()
    
    tb.connect (src,src_head,s2fsmi,enc_out,inter,enc_in,mod)
    tb.connect (mod,(add,0))
    tb.connect (noise,(add,1))
    tb.connect (add,metrics_in)
    tb.connect (gnd,(siso_in,0))
    tb.connect (metrics_in,(siso_in,1))
    tb.connect (siso_in,deinter,va_out,fsmi2s,dst)

    tb.run()
    
    ntotal = dst.ntotal ()
    nright = dst.nright ()
    runlength = dst.runlength ()
    return (ntotal,ntotal-nright)


def main(args):
    nargs = len (args)
    if nargs == 4:
        fname_out=args[0]
        fname_in=args[1]
        esn0_db=float(args[2]) # Es/No in dB
        rep=int(args[3]) # number of times the experiment is run to collect enough errors
    else:
        sys.stderr.write ('usage: test_tcm.py fsm_name_out fsm_fname_in Es/No_db  repetitions\n')
        sys.exit (1)

    # system parameters
    Kb=1024*16  # packet size in bits (make it multiple of 16 so it can be packed in a short)
    fo=trellis.fsm(fname_out) # get the outer FSM specification from a file
    fi=trellis.fsm(fname_in) # get the innner FSM specification from a file
    bitspersymbol = int(round(math.log(fo.I())/math.log(2))) # bits per FSM input symbol
    if fo.O() != fi.I():
        sys.stderr.write ('Incompatible cardinality between outer and inner FSM.\n')
        sys.exit (1)
    K=Kb/bitspersymbol # packet size in trellis steps
    interleaver=trellis.interleaver(K,666) # construct a random interleaver
    modulation = fsm_utils.psk8 # see fsm_utlis.py for available predefined modulations
    dimensionality = modulation[0]
    constellation = modulation[1] 
    if len(constellation)/dimensionality != fi.O():
        sys.stderr.write ('Incompatible FSM output cardinality and modulation size.\n')
        sys.exit (1)
    # calculate average symbol energy
    Es = 0
    for i in range(len(constellation)):
        Es = Es + constellation[i]**2
    Es = Es / (len(constellation)/dimensionality)
    N0=Es/pow(10.0,esn0_db/10.0); # calculate noise variance


    tot_s=0 # total number of transmitted shorts
    terr_s=0 # total number of shorts in error
    terr_p=0 # total number of packets in error
    for i in range(rep):
        (s,e)=run_test(fo,fi,interleaver,Kb,bitspersymbol,K,dimensionality,constellation,N0,-long(666+i)) # run experiment with different seed to get different noise realizations
        tot_s=tot_s+s
        terr_s=terr_s+e
        terr_p=terr_p+(terr_s!=0)
        if ((i+1)%100==0) : # display progress
            print i+1,terr_p, '%.2e' % ((1.0*terr_p)/(i+1)),tot_s,terr_s, '%.2e' % ((1.0*terr_s)/tot_s)
    # estimate of the (short or bit) error rate
    print rep,terr_p, '%.2e' % ((1.0*terr_p)/(i+1)),tot_s,terr_s, '%.2e' % ((1.0*terr_s)/tot_s)
    


if __name__ == '__main__':
    main (sys.argv[1:])

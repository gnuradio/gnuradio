#!/usr/bin/env python

from gnuradio import gr
from gnuradio import trellis, digital
from gnuradio import eng_notation
import math
import sys
import random
import fsm_utils

def make_rx(tb,fo,fi,dimensionality,tot_constellation,K,interleaver,IT,Es,N0,type):
    metrics_in = trellis.metrics_f(fi.O(),dimensionality,tot_constellation,digital.TRELLIS_EUCLIDEAN) # data preprocessing to generate metrics for innner SISO
    scale = gr.multiply_const_ff(1.0/N0)
    gnd = gr.vector_source_f([0],True);

    inter=[]
    deinter=[]
    siso_in=[]
    siso_out=[]

    # generate all blocks
    for it in range(IT):
      inter.append( trellis.permutation(interleaver.K(),interleaver.INTER(),fi.I(),gr.sizeof_float) )
      siso_in.append( trellis.siso_f(fi,K,0,-1,True,False,type) )
      deinter.append( trellis.permutation(interleaver.K(),interleaver.DEINTER(),fi.I(),gr.sizeof_float) )
      if it < IT-1:
        siso_out.append( trellis.siso_f(fo,K,0,-1,False,True,type) )
      else:
        siso_out.append( trellis.viterbi_s(fo,K,0,-1) ) # no soft outputs needed

    # connect first stage
    tb.connect (gnd,inter[0])
    tb.connect (metrics_in,scale)
    tb.connect (scale,(siso_in[0],1))

    # connect the rest
    for it in range(IT):
      if it < IT-1:
        tb.connect (scale,(siso_in[it+1],1))
        tb.connect (siso_in[it],deinter[it],(siso_out[it],1))
        tb.connect (gnd,(siso_out[it],0))
        tb.connect (siso_out[it],inter[it+1])
        tb.connect (inter[it],(siso_in[it],0))
      else:
        tb.connect (siso_in[it],deinter[it],siso_out[it])
        tb.connect (inter[it],(siso_in[it],0))

    return (metrics_in,siso_out[IT-1])


def run_test (fo,fi,interleaver,Kb,bitspersymbol,K,channel,modulation,dimensionality,tot_constellation,Es,N0,IT,seed):
    tb = gr.top_block ()
    L = len(channel)

    # TX
    # this for loop is TOO slow in python!!!
    packet = [0]*(K)
    random.seed(seed)
    for i in range(len(packet)):
        packet[i] = random.randint(0, 2**bitspersymbol - 1) # random symbols
    src = gr.vector_source_s(packet,False)
    enc_out = trellis.encoder_ss(fo,0) # initial state = 0
    inter = trellis.permutation(interleaver.K(),interleaver.INTER(),1,gr.sizeof_short)
    mod = gr.chunks_to_symbols_sf(modulation[1],modulation[0])

    # CHANNEL
    isi = gr.fir_filter_fff(1,channel)
    add = gr.add_ff()
    noise = gr.noise_source_f(gr.GR_GAUSSIAN,math.sqrt(N0/2),seed)
    
    # RX
    (head,tail) = make_rx(tb,fo,fi,dimensionality,tot_constellation,K,interleaver,IT,Es,N0,trellis.TRELLIS_MIN_SUM) 
    dst = gr.vector_sink_s(); 
    
    tb.connect (src,enc_out,inter,mod)
    tb.connect (mod,isi,(add,0))
    tb.connect (noise,(add,1))
    tb.connect (add,head)
    tb.connect (tail,dst)
    
    tb.run()

    data = dst.data()
    ntotal = len(data)
    nright=0
    for i in range(ntotal):
        if packet[i]==data[i]:
            nright=nright+1
        #else:
            #print "Error in ", i
 
    return (ntotal,ntotal-nright)




def main(args):
    nargs = len (args)
    if nargs == 3:
        fname_out=args[0]
        esn0_db=float(args[1])
        rep=int(args[2])
    else:
        sys.stderr.write ('usage: test_turbo_equalization.py fsm_name_out Es/No_db  repetitions\n')
        sys.exit (1)

    # system parameters
    Kb=64*16  # packet size in bits (multiple of 16)
    modulation = fsm_utils.pam4 # see fsm_utlis.py for available predefined modulations
    channel = fsm_utils.c_channel # see fsm_utlis.py for available predefined test channels
    fo=trellis.fsm(fname_out) # get the outer FSM specification from a file
    fi=trellis.fsm(len(modulation[1]),len(channel)) # generate the FSM automatically
    if fo.O() != fi.I():
        sys.stderr.write ('Incompatible cardinality between outer and inner FSM.\n')
        sys.exit (1)
    bitspersymbol = int(round(math.log(fo.I())/math.log(2))) # bits per FSM input symbol
    K=Kb/bitspersymbol # packet size in trellis steps
    interleaver=trellis.interleaver(K,666) # construct a random interleaver
    tot_channel = fsm_utils.make_isi_lookup(modulation,channel,True) # generate the lookup table (normalize energy to 1)
    dimensionality = tot_channel[0]
    tot_constellation = tot_channel[1]
    if len(tot_constellation)/dimensionality != fi.O():
        sys.stderr.write ('Incompatible FSM output cardinality and lookup table size.\n')
        sys.exit (1)
    N0=pow(10.0,-esn0_db/10.0); # noise variance
    IT = 3 # number of turbo iterations

    tot_s=0 # total number of transmitted shorts
    terr_s=0 # total number of shorts in error
    terr_p=0 # total number of packets in error

    for i in range(rep):
        (s,e)=run_test(fo,fi,interleaver,Kb,bitspersymbol,K,channel,modulation,dimensionality,tot_constellation,1,N0,IT,-long(666+i)) # run experiment with different seed to get different noise realizations
        tot_s=tot_s+s
        terr_s=terr_s+e
        terr_p=terr_p+(terr_s!=0)
        if ((i+1)%10==0) : # display progress
            print i+1,terr_p, '%.2e' % ((1.0*terr_p)/(i+1)),tot_s,terr_s, '%.2e' % ((1.0*terr_s)/tot_s)
    # estimate of the (short or bit) error rate
    print rep,terr_p, '%.2e' % ((1.0*terr_p)/(i+1)),tot_s,terr_s, '%.2e' % ((1.0*terr_s)/tot_s)



if __name__ == '__main__':
    main (sys.argv[1:])


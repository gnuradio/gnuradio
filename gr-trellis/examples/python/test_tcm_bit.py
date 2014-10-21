#!/usr/bin/env python

from gnuradio import gr
from gnuradio import trellis, digital, blocks
from gnuradio import eng_notation
import math
import sys
import random
import fsm_utils
from gnuradio.eng_option import eng_option
from optparse import OptionParser

try:
    from gnuradio import analog
except ImportError:
    sys.stderr.write("Error: Program requires gr-analog.\n")
    sys.exit(1)

def run_test (f,Kb,bitspersymbol,K,dimensionality,constellation,N0,seed):
    tb = gr.top_block ()

    # TX
    packet = [0]*Kb
    # this for loop is TOO slow!!!
    for i in range(Kb-1*16): # last 16 bits = 0 to drive the final state to 0
        packet[i] = random.randint(0, 1) # random 0s and 1s
    src = blocks.vector_source_s(packet,False)
    #src = blocks.lfsr_32k_source_s()
    #src_head = blocks.head (gr.sizeof_short,Kb/16) # packet size in shorts
    b2s = blocks.unpacked_to_packed_ss(1,gr.GR_MSB_FIRST) # pack bits in shorts
    s2fsmi = blocks.packed_to_unpacked_ss(bitspersymbol,gr.GR_MSB_FIRST) # unpack shorts to symbols compatible with the FSM input cardinality
    enc = trellis.encoder_ss(f,0) # initial state = 0
    mod = digital.chunks_to_symbols_sf(constellation,dimensionality)


    # CHANNEL
    add = blocks.add_ff()
    noise = analog.noise_source_f(analog.GR_GAUSSIAN,math.sqrt(N0/2),seed)


    # RX
    metrics = trellis.metrics_f(f.O(),dimensionality,constellation,digital.TRELLIS_EUCLIDEAN) # data preprocessing to generate metrics for Viterbi
    va = trellis.viterbi_s(f,K,0,-1) # Put -1 if the Initial/Final states are not set.
    fsmi2s = blocks.unpacked_to_packed_ss(bitspersymbol,gr.GR_MSB_FIRST) # pack FSM input symbols to shorts
    s2b = blocks.packed_to_unpacked_ss(1,gr.GR_MSB_FIRST) # unpack shorts to bits
    dst = blocks.vector_sink_s();
    #dst = blocks.check_lfsr_32k_s();


    #tb.connect (src,src_head,s2fsmi,enc,mod)
    tb.connect (src,b2s,s2fsmi,enc,mod)
    tb.connect (mod,(add,0))
    tb.connect (noise,(add,1))
    tb.connect (add,metrics)
    #tb.connect (metrics,va,fsmi2s,dst)
    tb.connect (metrics,va,fsmi2s,s2b,dst)


    tb.run()

    # A bit of cheating: run the program once and print the
    # final encoder state..
    # Then put it as the last argument in the viterbi block
    #print "final state = " , enc.ST()

    #ntotal = dst.ntotal ()
    #nright = dst.nright ()
    #runlength = dst.runlength ()
    ntotal = len(packet)
    if len(dst.data()) != ntotal:
        print "Error: not enough data\n"
    nright = 0;
    # this for loop is TOO slow!!!
    for i in range(ntotal):
        if packet[i]==dst.data()[i]:
            nright=nright+1
        #else:
            #print "Error in ", i
    return (ntotal,ntotal-nright)




def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-f", "--fsm_file", type="string", default="fsm_files/awgn1o2_4.fsm", help="Filename containing the fsm specification, e.g. -f fsm_files/awgn1o2_4.fsm (default=fsm_files/awgn1o2_4.fsm)")
    parser.add_option("-e", "--esn0", type="eng_float", default=10.0, help="Symbol energy to noise PSD level ratio in dB, e.g., -e 10.0 (default=10.0)")
    parser.add_option("-r", "--repetitions", type="int", default=100, help="Number of packets to be generated for the simulation, e.g., -r 100 (default=100)")

    (options, args) = parser.parse_args ()
    if len(args) != 0:
        parser.print_help()
        raise SystemExit, 1

    fname=options.fsm_file
    esn0_db=float(options.esn0)
    rep=int(options.repetitions)


    # system parameters
    f=trellis.fsm(fname) # get the FSM specification from a file
    Kb=1024*16  # packet size in bits (make it multiple of 16 so it can be packed in a short)
    bitspersymbol = int(round(math.log(f.I())/math.log(2))) # bits per FSM input symbol
    K=Kb/bitspersymbol # packet size in trellis steps
    modulation = fsm_utils.psk4 # see fsm_utlis.py for available predefined modulations
    dimensionality = modulation[0]
    constellation = modulation[1]
    if len(constellation)/dimensionality != f.O():
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
        (s,e)=run_test(f,Kb,bitspersymbol,K,dimensionality,constellation,N0,-long(666+i)) # run experiment with different seed to get different noise realizations
        tot_s=tot_s+s
        terr_s=terr_s+e
        terr_p=terr_p+(terr_s!=0)
        if ((i+1)%1==0) : # display progress
            print i+1,terr_p, '%.2e' % ((1.0*terr_p)/(i+1)),tot_s,terr_s, '%.2e' % ((1.0*terr_s)/tot_s)
    # estimate of the (short or bit) error rate
    print rep,terr_p, '%.2e' % ((1.0*terr_p)/(i+1)),tot_s,terr_s, '%.2e' % ((1.0*terr_s)/tot_s)



if __name__ == '__main__':
    main()

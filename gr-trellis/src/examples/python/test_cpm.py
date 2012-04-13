#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: CPM test
# Author: Achilleas Anastasopoulos
# Description: gnuradio flow graph
# Generated: Thu Feb 19 23:16:23 2009
##################################################

from gnuradio import gr
from gnuradio import trellis, digital
from gnuradio.gr import firdes
from grc_gnuradio import blks2 as grc_blks2
import math
import numpy
import fsm_utils
from gnuradio import trellis

try:
	import scipy.stats
except ImportError:
	print "Error: Program requires scipy (see: www.scipy.org)."
	sys.exit(1)

def run_test(seed,blocksize):
        tb = gr.top_block()

	##################################################
	# Variables
	##################################################
	M = 2
	K = 1
	P = 2
	h = (1.0*K)/P
	L = 3
	Q = 4
        frac = 0.99
        f = trellis.fsm(P,M,L)

        # CPFSK signals
        #p = numpy.ones(Q)/(2.0)
        #q = numpy.cumsum(p)/(1.0*Q)

        # GMSK signals
        BT=0.3;
        tt=numpy.arange(0,L*Q)/(1.0*Q)-L/2.0;
        #print tt
        p=(0.5*scipy.stats.erfc(2*math.pi*BT*(tt-0.5)/math.sqrt(math.log(2.0))/math.sqrt(2.0))-0.5*scipy.stats.erfc(2*math.pi*BT*(tt+0.5)/math.sqrt(math.log(2.0))/math.sqrt(2.0)))/2.0;
        p=p/sum(p)*Q/2.0;
        #print p
        q=numpy.cumsum(p)/Q;
        q=q/q[-1]/2.0;
        #print q

        (f0T,SS,S,F,Sf,Ff,N) = fsm_utils.make_cpm_signals(K,P,M,L,q,frac)
        #print N
        #print Ff
        Ffa = numpy.insert(Ff,Q,numpy.zeros(N),axis=0)
        #print Ffa
        MF = numpy.fliplr(numpy.transpose(Ffa))
        #print MF
        E = numpy.sum(numpy.abs(Sf)**2,axis=0)
        Es = numpy.sum(E)/f.O()
        #print Es

        constellation = numpy.reshape(numpy.transpose(Sf),N*f.O())
        #print Ff
        #print Sf
        #print constellation
        #print numpy.max(numpy.abs(SS - numpy.dot(Ff , Sf)))

	EsN0_db = 10.0
        N0 =  Es * 10.0**(-(1.0*EsN0_db)/10.0)
        #N0 = 0.0
        #print N0
        head = 4
        tail = 4
        numpy.random.seed(seed*666)
        data = numpy.random.randint(0, M, head+blocksize+tail+1)
        #data = numpy.zeros(blocksize+1+head+tail,'int')
        for i in range(head):
            data[i]=0
        for i in range(tail+1):
            data[-i]=0



	##################################################
	# Blocks
	##################################################
	random_source_x_0 = gr.vector_source_b(data.tolist(), False)
	gr_chunks_to_symbols_xx_0 = gr.chunks_to_symbols_bf((-1, 1), 1)
	gr_interp_fir_filter_xxx_0 = gr.interp_fir_filter_fff(Q, p)
	gr_frequency_modulator_fc_0 = gr.frequency_modulator_fc(2*math.pi*h*(1.0/Q))

	gr_add_vxx_0 = gr.add_vcc(1)
	gr_noise_source_x_0 = gr.noise_source_c(gr.GR_GAUSSIAN, (N0/2.0)**0.5, -long(seed))

	gr_multiply_vxx_0 = gr.multiply_vcc(1)
	gr_sig_source_x_0 = gr.sig_source_c(Q, gr.GR_COS_WAVE, -f0T, 1, 0)
        # only works for N=2, do it manually for N>2...
	gr_fir_filter_xxx_0_0 = gr.fir_filter_ccc(Q, MF[0].conjugate())
	gr_fir_filter_xxx_0_0_0 = gr.fir_filter_ccc(Q, MF[1].conjugate())
	gr_streams_to_stream_0 = gr.streams_to_stream(gr.sizeof_gr_complex*1, int(N))
	gr_skiphead_0 = gr.skiphead(gr.sizeof_gr_complex*1, int(N*(1+0)))
	viterbi = trellis.viterbi_combined_cb(f, head+blocksize+tail, 0, -1, int(N),
					      constellation, digital.TRELLIS_EUCLIDEAN)

        gr_vector_sink_x_0 = gr.vector_sink_b()

	##################################################
	# Connections
	##################################################
	tb.connect((random_source_x_0, 0), (gr_chunks_to_symbols_xx_0, 0))
	tb.connect((gr_chunks_to_symbols_xx_0, 0), (gr_interp_fir_filter_xxx_0, 0))
	tb.connect((gr_interp_fir_filter_xxx_0, 0), (gr_frequency_modulator_fc_0, 0))
	tb.connect((gr_frequency_modulator_fc_0, 0), (gr_add_vxx_0, 0))
	tb.connect((gr_noise_source_x_0, 0), (gr_add_vxx_0, 1))
	tb.connect((gr_add_vxx_0, 0), (gr_multiply_vxx_0, 0))
	tb.connect((gr_sig_source_x_0, 0), (gr_multiply_vxx_0, 1))
	tb.connect((gr_multiply_vxx_0, 0), (gr_fir_filter_xxx_0_0, 0))
	tb.connect((gr_multiply_vxx_0, 0), (gr_fir_filter_xxx_0_0_0, 0))
	tb.connect((gr_fir_filter_xxx_0_0, 0), (gr_streams_to_stream_0, 0))
	tb.connect((gr_fir_filter_xxx_0_0_0, 0), (gr_streams_to_stream_0, 1))
	tb.connect((gr_streams_to_stream_0, 0), (gr_skiphead_0, 0))
	tb.connect((gr_skiphead_0, 0), (viterbi, 0))
	tb.connect((viterbi, 0), (gr_vector_sink_x_0, 0))


        tb.run()
        dataest = gr_vector_sink_x_0.data()
        #print data
        #print numpy.array(dataest)
        perr = 0
        err = 0
        for i in range(blocksize):
          if data[head+i] != dataest[head+i]:
            #print i
            err += 1
        if err != 0 :
          perr = 1
        return (err,perr)

if __name__ == '__main__':
        blocksize = 1000
        ss=0
        ee=0
        for i in range(10000):
            (s,e) = run_test(i,blocksize)
            ss += s
            ee += e
            if (i+1) % 100 == 0:
                print i+1,ss,ee,(1.0*ss)/(i+1)/(1.0*blocksize),(1.0*ee)/(i+1)
        print i+1,ss,ee,(1.0*ss)/(i+1)/(1.0*blocksize),(1.0*ee)/(i+1)

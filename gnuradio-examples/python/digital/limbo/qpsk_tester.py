#!/usr/bin/env python

import random
from gnuradio import gr

default_access_code = '\xAC\xDD\xA4\xE2\xF2\x8C\x20\xFC'

def string_to_1_0_list(s):
    r = []
    for ch in s:
        x = ord(ch)
        for i in range(8):
            t = (x >> i) & 0x1
            r.append(t)

    return r

def to_1_0_string(L):
    return ''.join(map(lambda x: chr(x + ord('0')), L))

code = string_to_1_0_list(default_access_code)
access_code = to_1_0_string(code)

use_agc        = 0
use_rrc_tx     = 1
use_rrc_rx     = 1
use_sync_loop  = 1
use_clock_sync = 1

def main():
    fg = gr.flow_graph()

#    data = (1,2,3,4,5,6,7,8,9)
    random.seed()
    data = [random.randint(1,100) for i in range(20000)]
    data[0] = 0 # you know, for the diff encoding stuff
    bytes_src = gr.vector_source_b(data,False)

    k = 2
    spb = 50
    ntaps = 11*spb
    excess_bw = 0.9
    threshold = 12
    constellation  = (1+0j, 0+1j, -1+0j, 0-1j)
    
    bytes2chunks   = gr.packed_to_unpacked_bb(k, gr.GR_MSB_FIRST)
    diffenc        = gr.diff_encoder_bb(4)
    chunks2symbols = gr.chunks_to_symbols_bc(constellation)

    if use_rrc_tx:
        rrc_taps_tx = gr.firdes.root_raised_cosine(spb, spb, 1.0, \
                                                   excess_bw, ntaps)
        rrc_tx = gr.interp_fir_filter_ccf(spb, rrc_taps_tx)
    else:
        rrc_tx = gr.interp_fir_filter_ccf(1, (1,))

################### CHANNEL MODEL #############################

    phase_rotate = gr.multiply_const_cc(1-0.36j)
    channel      = gr.add_cc()
    awgn         = gr.noise_source_c(gr.GR_GAUSSIAN, 0.5)
    fg.connect(awgn, (channel,1))
    
################### CHANNEL MODEL #############################

    if use_agc:
        agc = gr.agc_cc(1e-4, 1, 1)
    else:
        agc = gr.multiply_const_cc(1)

    # Downconverter
    if use_sync_loop:
        costas_alpha=0.005
        beta = costas_alpha*costas_alpha*0.25
        sync_loop = gr.costas_loop_cc(costas_alpha, beta, 0.05, -0.05, 0)
    else:
        sync_loop = gr.multiply_const_cc(1)

    if use_rrc_rx:
        rrc_taps_rx = gr.firdes.root_raised_cosine(1, spb, 1.0, \
                                                   excess_bw, ntaps)
        rrc_rx = gr.fir_filter_ccf(1, rrc_taps_rx)
    else:
        rrc_rx = gr.fir_filter_ccf(1, (1,))
   
    # Sampler
    if use_clock_sync:
        mu = 0.05
        omega = spb
        gain_mu = 0.05
        gain_omega = 0.25*gain_mu*gain_mu
        omega_rel_limit = 0.5
        clock_sync = gr.clock_recovery_mm_cc(omega, gain_omega, mu, \
                                             gain_mu, omega_rel_limit)
        #clock_sync.set_verbose(True);
    else:
        clock_sync = gr.fir_filter_ccf(1, (1,))

    diff_phasor = gr.diff_phasor_cc()
    slicer = gr.constellation_decoder_cb((constellation), (0,1,2,3))
    unpack = gr.unpack_k_bits_bb(k)
    access = gr.correlate_access_code_bb(access_code,threshold)

    sink = gr.file_sink(gr.sizeof_char, 'output.dat')

    fg.connect(bytes_src, bytes2chunks, diffenc, chunks2symbols, rrc_tx)
    fg.connect(rrc_tx, phase_rotate, channel, agc)
    fg.connect(agc, sync_loop, rrc_rx, clock_sync, diff_phasor, slicer, sink)
 
    test = gr.file_sink(gr.sizeof_gr_complex, 'test.dat')
    fg.connect(rrc_rx, test)

    fg.connect(chunks2symbols, gr.file_sink(gr.sizeof_gr_complex, 'rrc_tx.dat'))  # into TX RRC
    fg.connect(channel, gr.file_sink(gr.sizeof_gr_complex, 'channel.dat'))        # Out of TX RRC
    fg.connect(rrc_rx, gr.file_sink(gr.sizeof_gr_complex, 'rrc_rx.dat'))          # Out of RX RRC -> clock_sync
    fg.connect(clock_sync, gr.file_sink(gr.sizeof_gr_complex, 'clock_sync.dat'))  # Out of M&M sync loop
    fg.connect(bytes2chunks, gr.file_sink(gr.sizeof_char, 'source.dat'))

    fg.start()
    fg.wait()

if __name__ == "__main__":
    main()
        
    

 

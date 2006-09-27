#!/usr/bin/env python

import random
from gnuradio import gr, gru, usrp

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

def main():

    fg = gr.flow_graph()
    f_rf = 5e6
    fs = 400e3
    sr = 100e3
    alpha = 0.5
    M = 4
    k = int(gru.log2(M))
    
    # Source
    src = usrp.source_c ()
    adc_rate = src.adc_rate()
    usrp_decim = int(adc_rate / fs)
    src.set_decim_rate(usrp_decim)
        
    subdev_spec = usrp.pick_rx_subdevice(src)
    subdev = usrp.selected_subdev(src, subdev_spec)
    print "Using RX d'board %s" % (subdev.side_and_name(),)
    src.set_mux(usrp.determine_rx_mux_value(src, subdev_spec))
    src.tune(0, subdev, f_rf)
        
    g = subdev.gain_range()
    subdev.set_gain(g[1])
    subdev.set_auto_tr(True)
        
    print "USRP Decimation Rate = %d" % usrp_decim
    print "RF Frequency = %d" % f_rf

    agc = gr.multiply_const_cc(0.0025)
        
    # Downconverter
    costas_alpha=0.005
    beta = costas_alpha*costas_alpha*0.25
    sync_loop = gr.costas_loop_cc(costas_alpha, beta, 0.05, -0.05, 0)

    # Stage 2
#    fs2 = 200e3
#    D = int(fs / fs2)
#    decimator = gr.keep_one_in_n(gr.sizeof_gr_complex, D)
#    print "D = %d\nAfter decimator fs = %f" % (D, fs2)

    # Demodulator
    taps = gr.firdes.root_raised_cosine(1, fs, sr, alpha, 45)
    rrc  = gr.fir_filter_ccf(1, taps)
    
    # Sampler
    mu = 0.01
    omega = 4.3
    gain_mu = 0.05
    gain_omega = 0.25*gain_mu*gain_mu
    omega_rel_limit = 0.5
    clock_sync = gr.clock_recovery_mm_cc(omega, gain_omega, mu,
                                         gain_mu, omega_rel_limit)
    clock_sync.set_verbose(False)

    diff_phasor = gr.diff_phasor_cc()

    threshold = 12
    constellation = (1+0j, 0+1j, -1+0j, 0-1j)
    slicer = gr.constellation_decoder_cb((constellation), (0,1,2,3))
    unpack = gr.unpack_k_bits_bb(k)
    access = gr.correlate_access_code_bb(access_code,threshold)

    test = gr.file_sink(gr.sizeof_gr_complex, 'test.dat')
    sink = gr.file_sink(gr.sizeof_char, 'output.dat')

    fg.connect(src, agc, sync_loop, rrc, clock_sync)
    fg.connect(clock_sync, diff_phasor, slicer, unpack, access, sink)

    fg.connect(slicer, gr.file_sink(gr.sizeof_char, 'chunks.dat'))
    fg.connect(unpack, gr.file_sink(gr.sizeof_char, 'unpack.dat'))
    fg.connect(clock_sync, gr.file_sink(gr.sizeof_gr_complex, 'phasor.dat'))

    fg.start()
    fg.wait()

if __name__ == "__main__":
    main()

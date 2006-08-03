#!/usr/bin/env python

from gnuradio import gr
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser



def build_graph ():

    # interp = 32
    interp = 64
    nchan = 2
    
    if nchan == 1:
        mux = 0x0098
        #mux = 0x9800
    else:
        mux = 0xba98

    f0 = 100e3
    a0 = 16e3
    duc0 = 5e6

    f1 = 50e3
    a1 = 16e3
    duc1 = 7e6

    fg = gr.flow_graph ()

    u = usrp.sink_c (0, interp, nchan, mux)
    sample_rate = u.dac_freq () / interp
    print "sample_rate = ", eng_notation.num_to_str (sample_rate)
    print "usb_sample_rate = ", eng_notation.num_to_str (sample_rate * nchan)

    u.set_tx_freq (0, duc0)
    u.set_tx_freq (1, duc1)

    interleave = gr.interleave (gr.sizeof_gr_complex)
    fg.connect (interleave, u)

    src0 = gr.sig_source_c (sample_rate, gr.GR_SIN_WAVE, f0, a0, 0)
    fg.connect (src0, (interleave, 0))

    if nchan == 2:
        if 1:
            src1 = gr.sig_source_c (sample_rate, gr.GR_SIN_WAVE, f1, a1, 0)
        else:
            src1 = gr.noise_source_c (gr.GR_UNIFORM, a1)
        fg.connect (src1, (interleave, 1))
    
    return fg


if __name__ == '__main__':
    fg = build_graph ()
    fg.start ()
    raw_input ('Press Enter to quit: ')
    fg.stop ()


#!/usr/bin/env python

from gnuradio import gr, eng_notation
from gnuradio import usrp
from gnuradio import audio
from gnuradio.eng_option import eng_option
from optparse import OptionParser


def build_graph (cordic_freq):

    audio_rate = 32000
    sw_interp = 4
    usrp_interp = 1000
    audio_file = "/home/eb/demo/testing-audio.dat"
    
    fg = gr.flow_graph ()

    src = gr.file_source (gr.sizeof_float, audio_file, True)
    gain = gr.multiply_const_ff (10000)

    interp = gr.interp_fir_filter_fff (sw_interp, (1, 1, 1, 1))

    f2c = gr.float_to_complex ()
    
    u = usrp.sink_c (0, usrp_interp)
    u.set_tx_freq (0, cordic_freq)

    fg.connect (src, gain)
    fg.connect (gain, interp)
    fg.connect (interp, (f2c, 0))
    fg.connect (interp, (f2c, 1))
    fg.connect (f2c, u)

    return fg

def main ():
    parser = OptionParser (option_class=eng_option)
    parser.add_option ("-c", "--cordic-freq", type="eng_float", default=10e6,
                       help="set Tx cordic frequency to FREQ", metavar="FREQ")
    (options, args) = parser.parse_args ()

    print "cordic_freq = %s" % (eng_notation.num_to_str (options.cordic_freq))
    fg = build_graph (options.cordic_freq)
    
    fg.start ()
    raw_input ('Press Enter to quit: ')
    fg.stop ()

if __name__ == '__main__':
    main ()

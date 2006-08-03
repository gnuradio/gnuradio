#!/usr/bin/env python

from gnuradio import gr, eng_notation
from gnuradio import audio
# from gnuradio import usrp
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys
import math


#
# return a gr.flow_graph
#
def build_graph (input_filename, repeat):
    adc_rate = 64e6                             # USRP A/D sampling rate
    decim = 250                                 # FPGA decimated by this amount

    quad_rate = adc_rate / decim                # 256 kHz (the sample rate of the file)
    audio_decimation = 8
    audio_rate = quad_rate / audio_decimation   # 32 kHz

    fg = gr.flow_graph ()
    
    # usrp is data source
    # src = usrp.source_c (0, decim)
    # src.set_rx_freq (0, -IF_freq)

    src = gr.file_source (gr.sizeof_gr_complex, input_filename, repeat)
    
    (head, tail) = build_pipeline (fg, quad_rate, audio_decimation)

    # sound card as final sink
    audio_sink = audio.sink (int (audio_rate))

    # now wire it all together
    fg.connect (src, head)
    fg.connect (tail, (audio_sink, 0))
    
    return fg

def build_pipeline (fg, quad_rate, audio_decimation):
    '''Given a flow_graph, fg, construct a pipeline
    for demodulating a broadcast FM signal.  The
    input is the downconverteed complex baseband
    signal. The output is the demodulated audio.

    build_pipeline returns a two element tuple
    containing the input and output endpoints.
    '''
    fm_demod_gain = 2200.0/32768.0
    audio_rate = quad_rate / audio_decimation
    volume = 1.0

    # input: complex; output: float
    fm_demod = gr.quadrature_demod_cf (volume*fm_demod_gain)

    # compute FIR filter taps for audio filter
    width_of_transition_band = audio_rate / 32
    audio_coeffs = gr.firdes.low_pass (1.0,            # gain
                                       quad_rate,      # sampling rate
                                       audio_rate/2 - width_of_transition_band,
                                       width_of_transition_band,
                                       gr.firdes.WIN_HAMMING)

    TAU  = 75e-6  # 75us in US, 50us in EUR
    fftaps = [ 1 - math.exp(-1/TAU/quad_rate), 0]
    fbtaps= [ 0 , math.exp(-1/TAU/quad_rate) ]
    deemph = gr.iir_filter_ffd(fftaps,fbtaps)

    # input: float; output: float
    audio_filter = gr.fir_filter_fff (audio_decimation, audio_coeffs)

    fg.connect (fm_demod, deemph)
    fg.connect (deemph, audio_filter)
    return ((fm_demod, 0), (audio_filter, 0))
    

def main ():
    usage = "usage: %prog [options] filename"
    parser = OptionParser (option_class=eng_option, usage=usage)
    parser.add_option ("-r", "--repeat", action="store_true", default=False)
    # parser.add_option (... your stuff here...)
    (options, args) = parser.parse_args ()

    if len (args) != 1:
        parser.print_help ()
        sys.exit (1)

    fg = build_graph (args[0], options.repeat)

    fg.start ()        # fork thread(s) and return
    raw_input ('Press Enter to quit: ')
    fg.stop ()

if __name__ == '__main__':
    main ()



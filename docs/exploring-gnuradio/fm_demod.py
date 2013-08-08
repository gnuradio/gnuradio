#!/usr/bin/env python

from gnuradio import gr
from gnuradio import analog
from gnuradio import audio
from gnuradio import mc4020
import sys

def high_speed_adc(fg, input_rate):
    # return blocks.file_source (gr.sizeof_short, "dummy.dat", False)
    return mc4020.source(input_rate, mc4020.MCC_CH3_EN | mc4020.MCC_ALL_1V)

#
# return a gr.flow_graph
#
def build_graph(freq1, freq2):
    input_rate = 20e6
    cfir_decimation = 125
    audio_decimation = 5

    quad_rate = input_rate / cfir_decimation
    audio_rate = quad_rate / audio_decimation

    fg = gr.flow_graph()

    # use high speed ADC as input source
    src = high_speed_adc(fg, input_rate)

    # compute FIR filter taps for channel selection
    channel_coeffs = \
        filter.firdes.low_pass(1.0,          # gain
                               input_rate,   # sampling rate
                               250e3,        # low pass cutoff freq
                               8*100e3,      # width of trans. band
                               filter.firdes.WIN_HAMMING)

    # input: short; output: complex
    chan_filter1 = \
        filter.freq_xlating_fir_filter_scf(cfir_decimation,
                                           channel_coeffs,
                                           freq1,        # 1st station freq
                                           input_rate)

    (head1, tail1) = build_pipeline(fg, quad_rate, audio_decimation)

    # sound card as final sink
    audio_sink = audio.sink(int (audio_rate))

    # now wire it all together
    fg.connect(src, chan_filter1)
    fg.connect(chan_filter1, head1)
    fg.connect(tail1, (audio_sink, 0))

    # two stations at once?
    if freq2:
        # Extract the second station and connect
        # it to a second pipeline...

        # input: short; output: complex
        chan_filter2 = \
            filter.freq_xlating_fir_filter_scf(cfir_decimation,
                                               channel_coeffs,
                                               freq2,        # 2nd station freq
                                               input_rate)

        (head2, tail2) = build_pipeline(fg, quad_rate, audio_decimation)

        fg.connect(src, chan_filter2)
        fg.connect(chan_filter2, head2)
        fg.connect(tail2, (audio_sink, 1))

    return fg

def build_pipeline(fg, quad_rate, audio_decimation):
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
    fm_demod = analog.quadrature_demod_cf(volume*fm_demod_gain)

    # compute FIR filter taps for audio filter
    width_of_transition_band = audio_rate / 32
    audio_coeffs = filter.firdes.low_pass(1.0,            # gain
                                          quad_rate,      # sampling rate
                                          audio_rate/2 - width_of_transition_band,
                                          width_of_transition_band,
                                          filter.firdes.WIN_HAMMING)

    # input: float; output: float
    audio_filter = filter.fir_filter_fff(audio_decimation, audiocoeffs)

    fg.connect(fm_demod, audio_filter)
    return ((fm_demod, 0), (audio_filter, 0))


def main(args):
    nargs = len(args)
    if nargs == 1:
        freq1 = float(args[0]) * 1e6
        freq2 = None
    elif nargs == 2:
        freq1 = float(args[0]) * 1e6
        freq2 = float(args[1]) * 1e6
    else:
        sys.stderr.write('usage: fm_demod freq1 [freq2]\n')
        sys.exit(1)

    # connect to RF front end
    rf_front_end = gr.microtune_4937_eval_board()
    if not rf_front_end.board_present_p():
        raise IOError, 'RF front end not found'

    # set front end gain
    rf_front_end.set_AGC(300)
    IF_freq = rf_front_end.get_output_freq()
    IF_freq = 5.75e6

    if not freq2:      # one station

        rf_front_end.set_RF_freq(freq1)
        fg = build_graph(IF_freq, None)

    else:              # two stations

        if abs(freq1 - freq2) > 5.5e6:
            raise IOError, 'freqs too far apart'

        target_freq = (freq1 + freq2) / 2
        actual_freq = rf_front_end.set_RF_freq(target_freq)
        #actual_freq = target_freq

        fg = build_graph(IF_freq + freq1 - actual_freq,
                         IF_freq + freq2 - actual_freq)

    fg.start()        # fork thread(s) and return
    raw_input('Press Enter to quit: ')
    fg.stop()

if __name__ == '__main__':
    main(sys.argv[1:])



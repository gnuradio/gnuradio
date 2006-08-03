#!/usr/bin/env python

from gnuradio import gr, gru, eng_notation, optfir
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import os.path
import re

plot = None

def split_filename(src_filename):
    s = os.path.splitext(src_filename)[0]     # drop extension
    date, time, freq, dec, nchan = s.split('-')
    return (date, time, freq, int(dec), int(nchan))


def make_filename(date, time, freq, n, short_output):
    if short_output:
        return '-'.join((date, time, freq)) + '.s%02d' % (n,)
    else:
        return '-'.join((date, time, freq)) + '.c%02d' % (n,)

class my_graph(gr.flow_graph):

    def __init__(self, src_filename, short_output):
        """
        Deinterleave file, filter and decimate by 4, and write out
        a separate file for each input channel.

        The input file is the raw output of nchannels of short USRP data.
        That is, they are interleaved short I&Q for each channel.
        """
        global plot
        
        gr.flow_graph.__init__(self)

        decim = 4

        (date, time, freq, dec, nchan) = split_filename(src_filename)

        src = gr.file_source(gr.sizeof_short, src_filename)

        # convert stream of interleaved shorts to a stream of complex
        s2c = gr.interleaved_short_to_complex()

        # deinterleave complexes into separate streams
        deint = gr.deinterleave(gr.sizeof_gr_complex)

        self.connect(src, s2c, deint)

        taps = optfir.low_pass(1,       # gain
                               1,       # sampling rate
                               0.100,   # passband cutoff
                               0.125,   # stopband cutoff
                               0.01,    # passband ripple (dB)
                               70)      # stopband atten (dB)

        print "len(taps) =", len(taps)
                               
        #plot = gru.gnuplot_freqz(gru.freqz(taps, 1), 1)
        #raw_input('Press Enter to continue: ')
        
        for n in xrange(nchan):
            #df = gr.fft_filter_ccc(decim, taps)
            df = gr.fir_filter_ccf(decim, taps)
            self.connect((deint, n), df)
            dst_filename = make_filename(date, time, freq, n, short_output)
            if short_output:
                c2s = gr.complex_to_interleaved_short()
                dst = gr.file_sink(gr.sizeof_short, dst_filename)
                self.connect(df, c2s, dst)
            else:
                dst = gr.file_sink(gr.sizeof_gr_complex, dst_filename)
                self.connect(df, dst)


def split_1_file(filename, short_output):
    my_graph(filename, short_output).run()


def main():
    usage="%prog: [options] file_to_split..."
    parser = OptionParser (option_class=eng_option, usage=usage)
    parser.add_option("-s", "--short", action="store_true", default=False,
                      help="short output if set, else complex")
    (options, args) = parser.parse_args()

    for filename in args:
        split_1_file(filename, options.short)


if __name__ == '__main__':
    main()

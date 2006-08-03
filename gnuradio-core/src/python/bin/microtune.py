#!/usr/bin/env python
# -*- Python -*-

from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import stdgui, fftsink
from optparse import OptionParser
from gnuradio import eng_notation


def main ():
    parser = OptionParser (option_class=eng_option)
    parser.add_option ("-g", "--gain", type="eng_float", default=-1,
                       help="set front end gain to GAIN [0,1000]")
    parser.add_option ("-f", "--freq", type="eng_float", default=-1,
                       help="set front end center frequency to FREQ")
    parser.add_option ("-t", "--type", type="string", default="4937",
                       help="select eval board type {4937 or 4702}")
    parser.add_option ("-p", "--port", type="int", default=0,
                       help="parallel port eval board is attached to")
    (options, args) = parser.parse_args ()

    if options.type == "4937":
        front_end = gr.microtune_4937_eval_board (options.port)
    elif options.type == "4702":
        front_end = gr.microtune_4702_eval_board (options.port)
    else:
        raise RuntimeError, "Invalid board type.  Must be either -t 4937 or -t 4702"

    if options.gain != -1:
        front_end.set_AGC (options.gain)

    if options.freq != -1:
        if options.freq < 1e6:
            options.freq = options.freq * 1e6

        actual = front_end.set_RF_freq (options.freq)
        print "microtune: actual freq = %s" %  (eng_notation.num_to_str (actual),)
        

if __name__ == '__main__':
    main ()

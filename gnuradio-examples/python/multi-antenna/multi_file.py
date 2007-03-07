#!/usr/bin/env python

from gnuradio import gr, gru, eng_notation
from gnuradio import usrp
from gnuradio.eng_option import eng_option
from gnuradio import eng_notation
from gnuradio import optfir
from optparse import OptionParser
from gnuradio.wxgui import stdgui, fftsink, waterfallsink, scopesink, form, slider
import wx
from usrpm import usrp_dbid
import time
import os.path
import sys

# required FPGA that can do 4 rx channels.


class my_graph(gr.flow_graph):

    def __init__(self):
        gr.flow_graph.__init__(self)

        parser = OptionParser (option_class=eng_option)
        #parser.add_option("-S", "--subdev", type="subdev", default=(0, None),
        #                  help="select USRP Rx side A or B (default=A)")
        parser.add_option("-d", "--decim", type="int", default=128,
                          help="set fgpa decimation rate to DECIM [default=%default]")
        parser.add_option("-f", "--freq", type="eng_float", default=146.585e6,
                          help="set frequency to FREQ [default=%default])", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=20,
                          help="set gain in dB [default=%default]")
        parser.add_option("-F", "--filter", action="store_true", default=True,
                          help="Enable channel filter")
        parser.add_option("-o", "--output", type="string", default=None,
                          help="set output basename")
        (options, args) = parser.parse_args()

        if len(args) != 0:
            parser.print_help()
            raise SystemExit

        if options.output is None:
            parser.print_help()
            sys.stderr.write("You must provide an output filename base with -o OUTPUT\n")
            raise SystemExit
        else:
            basename = options.output

        nchan = 4
        nsecs = 4.0

        if options.filter:
            sw_decim = 4
        else:
            sw_decim = 1

        self.u = usrp.source_c(0, options.decim, fpga_filename="std_4rx_0tx.rbf")
        if self.u.nddc() < nchan:
            sys.stderr.write('This code requires an FPGA build with %d DDCs.  This FPGA has only %d.\n' % (
                nchan, self.u.nddc()))
            raise SystemExit
                             
        if not self.u.set_nchannels(nchan):
            sys.stderr.write('set_nchannels(%d) failed\n' % (nchan,))
            raise SystemExit
        
        input_rate = self.u.adc_freq() / self.u.decim_rate()
        print "USB data rate   = %s" % (eng_notation.num_to_str(input_rate),)
        sink_data_rate = input_rate/sw_decim
        print "Scope data rate = %s" % (eng_notation.num_to_str(sink_data_rate),)

        self.subdev = self.u.db[0] + self.u.db[1]

        if (len(self.subdev) != 4 or
            self.u.db[0][0].dbid() != usrp_dbid.BASIC_RX or
            self.u.db[1][0].dbid() != usrp_dbid.BASIC_RX):
            sys.stderr.write('This code requires a Basic Rx board on Sides A & B\n')
            sys.exit(1)

        self.u.set_mux(gru.hexint(0xf3f2f1f0))

        # collect 1 second worth of data
        limit = int(nsecs * input_rate * nchan)
        print "limit = ", limit
        head = gr.head(gr.sizeof_gr_complex, limit)

        # deinterleave four channels from FPGA
        di = gr.deinterleave(gr.sizeof_gr_complex)

        self.connect(self.u, head, di)
        
        # taps for channel filter
        chan_filt_coeffs = optfir.low_pass (1,           # gain
                                            input_rate,  # sampling rate
                                            80e3,        # passband cutoff
                                            115e3,       # stopband cutoff
                                            0.1,         # passband ripple
                                            60)          # stopband attenuation
        #print len(chan_filt_coeffs)

        for i in range(nchan):

            sink = gr.file_sink(gr.sizeof_gr_complex,
                                basename + ("-%s-%d.dat" % (eng_notation.num_to_str(sink_data_rate), i)))
            if options.filter:
                chan_filt = gr.fir_filter_ccf(sw_decim, chan_filt_coeffs)
                self.connect((di, i), chan_filt, sink)
            else:
                self.connect((di, i), sink)


        self.set_gain(options.gain)
        self.set_freq(options.freq)

    def set_gain(self, gain):
        for i in range(len(self.subdev)):
            self.subdev[i].set_gain(gain)

    def set_freq(self, target_freq):
        ok = True
        for i in range(len(self.subdev)):
            r = usrp.tune(self.u, i, self.subdev[i], target_freq)
            if not r:
                ok = False
                print "set_freq: failed to set subdev[%d] freq to %f" % (
                    i, target_freq)

        return ok


def main ():
    my_graph().run()

if __name__ == '__main__':
    main ()

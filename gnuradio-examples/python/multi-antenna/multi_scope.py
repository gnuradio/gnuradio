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


class my_graph(stdgui.gui_flow_graph):

    def __init__(self, frame, panel, vbox, argv):
        stdgui.gui_flow_graph.__init__(self)

        self.frame = frame
        self.panel = panel

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
        (options, args) = parser.parse_args()

        if len(args) != 0:
            parser.print_help()
            raise SystemExit

        nchan = 4

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
        print "Scope data rate = %s" % (eng_notation.num_to_str(input_rate/sw_decim),)

        self.subdev = self.u.db[0] + self.u.db[1]

        if (len(self.subdev) != 4 or
            self.u.db[0][0].dbid() != usrp_dbid.BASIC_RX or
            self.u.db[1][0].dbid() != usrp_dbid.BASIC_RX):
            sys.stderr.write('This code requires a Basic Rx board on Sides A & B\n')
            sys.exit(1)

        self.u.set_mux(gru.hexint(0xf3f2f1f0))

        # deinterleave four channels from FPGA
        di = gr.deinterleave(gr.sizeof_gr_complex)

        self.connect(self.u, di)
        
        # our destination (8 float inputs)
        self.scope = scopesink.scope_sink_f(self, panel, sample_rate=input_rate/sw_decim)

        # taps for channel filter
        chan_filt_coeffs = optfir.low_pass (1,           # gain
                                            input_rate,  # sampling rate
                                            80e3,        # passband cutoff
                                            115e3,       # stopband cutoff
                                            0.1,         # passband ripple
                                            60)          # stopband attenuation
        #print len(chan_filt_coeffs)

        # bust the deinterleaved complex channels into floats
        for i in range(nchan):

            if options.filter:
                chan_filt = gr.fir_filter_ccf(sw_decim, chan_filt_coeffs)
                c2f = gr.complex_to_float()
                self.connect((di, i), chan_filt, c2f)
            else:
                c2f = gr.complex_to_float()
                self.connect((di, i), c2f)

            self.connect((c2f, 0), (self.scope, 2*i + 0))
            self.connect((c2f, 1), (self.scope, 2*i + 1))


        self._build_gui(vbox)

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


    def _build_gui(self, vbox):
        vbox.Add(self.scope.win, 10, wx.EXPAND)
        


def main ():
    app = stdgui.stdapp(my_graph, "Multi Scope", nstatus=1)
    app.MainLoop()

if __name__ == '__main__':
    main ()

#!/usr/bin/env python

from gnuradio import gr, gru, eng_notation
from gnuradio import usrp
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import usrp_dbid
import time
import os.path


def make_filename(dir, freq, decim, nchan, suffix):
    t = time.strftime('%Y%m%d-%H%M%S')
    f = 'R%s-%s-%d-%d.%s' % (
        t, eng_notation.num_to_str(freq), decim, nchan, suffix)
    return os.path.join(dir, f)

class radar_graph(gr.flow_graph):

    def __init__(self):
        gr.flow_graph.__init__(self)

        parser = OptionParser (option_class=eng_option)
        #parser.add_option("-S", "--subdev", type="subdev", default=(0, None),
        #                  help="select USRP Rx side A or B (default=A)")
        parser.add_option("-d", "--decim", type="int", default=64,
                          help="set fgpa decimation rate to DECIM (default=64)")
        parser.add_option("-f", "--freq", type="eng_float", default=104.5e6,
                          help="set frequency to FREQ (default=104.5M)", metavar="FREQ")
        parser.add_option("", "--gain0", type="eng_float", default=0,
                          help="set gain in dB (default 0)")
        parser.add_option("", "--gain1", type="eng_float", default=11,
                          help="set gain in dB (default 11)")
        parser.add_option("-F", "--filename", default=None)
        parser.add_option("-D", "--dir", default=None)
        (options, args) = parser.parse_args()

        if len(args) != 0:
            parser.print_help()
            sys.exit(1)

        if options.filename is None and options.dir is None:
            sys.stderr.write('Must specify either -F FILENAME or -D DIR\n')
            parser.print_help()
            sys.exit(1)

        nchan = 2

        # if filename was given, use it, otherwise build one.
        if options.filename is None:
            options.filename = make_filename(options.dir, options.freq,
                                             options.decim, nchan, 'srd')

        self.u = usrp.source_s(0, options.decim, nchan, 0, 0)
        self.subdev = self.u.db[0]

        if self.u.db[0][0].dbid() != usrp_dbid.BASIC_RX:
            sys.stderr.write('This code requires a Basic Rx board on Side A\n')
            sys.exit(1)

        # self.u.set_mux(usrp.determine_rx_mux_value(self.u, options.subdev))
        self.u.set_mux(gru.hexint(0xf0f0f1f0))

        dst = gr.file_sink (gr.sizeof_short, options.filename)
        self.connect (self.u, dst)

        self.subdev[0].set_gain(options.gain0)
        self.subdev[1].set_gain(options.gain1)
        self.set_freq(options.freq)


    def set_freq(self, target_freq):
        ok = True
        for i in range(len(self.subdev)):
            r = usrp.tune(self.u, i, self.subdev[i], target_freq)
            if not r:
                ok = False
                print "set_freq: failed to set subdev[%d] freq to %f" % (
                    i, target_freq)

        return ok


if __name__ == '__main__':
    fg = radar_graph()
    try:
        fg.run()
    except KeyboardInterrupt:
        pass

#!/usr/bin/env python

"""
Read samples from the USRP and write to file formatted as binary
outputs single precision complex float values or complex short values (interleaved 16 bit signed short integers).

"""

from gnuradio import gr, eng_notation
from gnuradio import audio
from gnuradio import usrp
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        usage="%prog: [options] output_filename"
        parser = OptionParser(option_class=eng_option, usage=usage)
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=(0, 0),
                          help="select USRP Rx side A or B (default=A)")
        parser.add_option("-d", "--decim", type="int", default=16,
                          help="set fgpa decimation rate to DECIM [default=%default]")
        parser.add_option("-f", "--freq", type="eng_float", default=None,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-8", "--width-8", action="store_true", default=False,
                          help="Enable 8-bit samples across USB")
        parser.add_option( "--no-hb", action="store_true", default=False,
                          help="don't use halfband filter in usrp")
        parser.add_option( "-s","--output-shorts", action="store_true", default=False,
                          help="output interleaved shorts in stead of complex floats")
        parser.add_option("-N", "--nsamples", type="eng_float", default=None,
                          help="number of samples to collect [default=+inf]")
        (options, args) = parser.parse_args ()
        if len(args) != 1:
            parser.print_help()
            raise SystemExit, 1
        filename = args[0]

        if options.freq is None:
            parser.print_help()
            sys.stderr.write('You must specify the frequency with -f FREQ\n');
            raise SystemExit, 1

        # build the graph
        if options.no_hb or (options.decim<8):
          self.fpga_filename="std_4rx_0tx.rbf" #Min decimation of this firmware is 4. contains 4 Rx paths without halfbands and 0 tx paths.
          if options.output_shorts:
            self.u = usrp.source_s(decim_rate=options.decim,fpga_filename=self.fpga_filename)
          else:
            self.u = usrp.source_c(decim_rate=options.decim,fpga_filename=self.fpga_filename)
        else:
          #standard fpga firmware "std_2rxhb_2tx.rbf" contains 2 Rx paths with halfband filters and 2 tx paths (the default) min decimation 8
          if options.output_shorts:
            self.u = usrp.source_s(decim_rate=options.decim)
          else:
            self.u = usrp.source_c(decim_rate=options.decim)
        if options.width_8:
            sample_width = 8
            sample_shift = 8
            format = self.u.make_format(sample_width, sample_shift)
            r = self.u.set_format(format)
        if options.output_shorts:
          self.dst = gr.file_sink(gr.sizeof_short, filename)
        else:
          self.dst = gr.file_sink(gr.sizeof_gr_complex, filename)
        if options.nsamples is None:
            self.connect(self.u, self.dst)
        else:
            if options.output_shorts:
              self.head = gr.head(gr.sizeof_short, int(options.nsamples)*2)
            else:
              self.head = gr.head(gr.sizeof_gr_complex, int(options.nsamples))
            self.connect(self.u, self.head, self.dst)

        if options.rx_subdev_spec is None:
            options.rx_subdev_spec = usrp.pick_rx_subdevice(self.u)
        self.u.set_mux(usrp.determine_rx_mux_value(self.u, options.rx_subdev_spec))

        # determine the daughterboard subdevice we're using
        self.subdev = usrp.selected_subdev(self.u, options.rx_subdev_spec)
        print "Using RX d'board %s" % (self.subdev.side_and_name(),)
        input_rate = self.u.adc_freq() / self.u.decim_rate()
        print "USB sample rate %s" % (eng_notation.num_to_str(input_rate))

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.subdev.gain_range()
            options.gain = float(g[0]+g[1])/2

        self.subdev.set_gain(options.gain)

        r = self.u.tune(0, self.subdev, options.freq)
        if not r:
            sys.stderr.write('Failed to set frequency\n')
            raise SystemExit, 1


if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass

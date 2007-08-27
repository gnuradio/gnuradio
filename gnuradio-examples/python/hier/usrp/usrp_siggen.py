#!/usr/bin/env python

from gnuradio import gr, gru
from gnuradio import usrp
from gnuradio.eng_option import eng_option
from gnuradio import eng_notation
from optparse import OptionParser
import sys

class my_graph(gr.top_block):
    def __init__ (self, type, ampl, wfreq, offset, subdev_spec, interp, rf_freq):
        gr.top_block.__init__(self, "usrp_siggen")
        
        # controllable values
        self.interp = interp
        self.waveform_type = type
        self.waveform_ampl = ampl
        self.waveform_freq = wfreq
        self.waveform_offset = offset

        self.u = usrp.sink_c (0, self.interp)

        # determine the daughterboard subdevice we're using
        if subdev_spec is None:
            ubdev_spec = usrp.pick_tx_subdevice(self.u)
        m = usrp.determine_tx_mux_value(self.u, subdev_spec)
        self.u.set_mux(m)
        self.subdev = usrp.selected_subdev(self.u, subdev_spec)
        self.subdev.set_gain(self.subdev.gain_range()[1])    # set max Tx gain
        self.subdev.set_enable(True)                         # enable transmitter
        print "Using TX d'board %s" % (self.subdev.side_and_name(),)

        if not self.set_freq(rf_freq):
            sys.stderr.write('Failed to set RF frequency\n')
            raise SystemExit

        if type == gr.GR_SIN_WAVE or type == gr.GR_CONST_WAVE:
            self.src = gr.sig_source_c (self.usb_freq (),
                                        gr.GR_SIN_WAVE,
                                        self.waveform_freq,
                                        self.waveform_ampl,
                                        self.waveform_offset)

        elif type == gr.GR_UNIFORM or type == gr.GR_GAUSSIAN:
            self.src = gr.noise_source_c (gr.GR_UNIFORM,
                                          self.waveform_ampl)

        else:
            raise ValueError, type

        self.connect (self.src, self.u)


    def usb_freq (self):
        return self.u.dac_freq() / self.interp

    def usb_throughput (self):
        return self.usb_freq () * 4
        
    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital up converter.
        """
        r = self.u.tune(self.subdev._which, self.subdev, target_freq)
        if r:
            #print "r.baseband_freq =", eng_notation.num_to_str(r.baseband_freq)
            #print "r.dxc_freq      =", eng_notation.num_to_str(r.dxc_freq)
            #print "r.residual_freq =", eng_notation.num_to_str(r.residual_freq)
            #print "r.inverted      =", r.inverted
            return True

        return False



def main ():
    parser = OptionParser (option_class=eng_option)
    parser.add_option ("-T", "--tx-subdev-spec", type="subdev", default=(0, 0),
                       help="select USRP Tx side A or B")
    parser.add_option ("-f", "--rf-freq", type="eng_float", default=None,
                       help="set RF center frequency to FREQ")
    parser.add_option ("-i", "--interp", type="int", default=64,
                       help="set fgpa interpolation rate to INTERP [default=%default]")

    parser.add_option ("--sine", dest="type", action="store_const", const=gr.GR_SIN_WAVE,
                       help="generate a complex sinusoid [default]", default=gr.GR_SIN_WAVE)
    parser.add_option ("--const", dest="type", action="store_const", const=gr.GR_CONST_WAVE, 
                       help="generate a constant output")
    parser.add_option ("--gaussian", dest="type", action="store_const", const=gr.GR_GAUSSIAN,
                       help="generate Gaussian random output")
    parser.add_option ("--uniform", dest="type", action="store_const", const=gr.GR_UNIFORM,
                       help="generate Uniform random output")

    parser.add_option ("-w", "--waveform-freq", type="eng_float", default=100e3,
                       help="set waveform frequency to FREQ [default=%default]")
    parser.add_option ("-a", "--amplitude", type="eng_float", default=16e3,
                       help="set waveform amplitude to AMPLITUDE [default=%default]", metavar="AMPL")
    parser.add_option ("-o", "--offset", type="eng_float", default=0,
                       help="set waveform offset to OFFSET [default=%default]")
    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help()
        raise SystemExit

    if options.rf_freq is None:
        sys.stderr.write("usrp_siggen: must specify RF center frequency with -f RF_FREQ\n")
        parser.print_help()
        raise SystemExit

    top_block = my_graph(options.type, options.amplitude, options.waveform_freq, options.offset,
                         options.tx_subdev_spec, options.interp, options.rf_freq)

    try:    
        # Run forever
        top_block.run()
    except KeyboardInterrupt:
        # Ctrl-C exits
        pass

if __name__ == '__main__':
    main ()

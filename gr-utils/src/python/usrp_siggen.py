#!/usr/bin/env python

from gnuradio import gr, gru
from gnuradio import usrp
from gnuradio.eng_option import eng_option
from gnuradio import eng_notation
from optparse import OptionParser
import sys


class my_top_block(gr.top_block):
    def __init__ (self):
        gr.top_block.__init__(self)
        
        # controllable values
        self.interp = 64
        self.waveform_type = gr.GR_SIN_WAVE
        self.waveform_ampl = 16000
        self.waveform_freq = 100.12345e3
        self.waveform_offset = 0
        self._instantiate_blocks ()
        self.set_waveform_type (self.waveform_type)

    def usb_freq (self):
        return self.u.dac_freq() / self.interp

    def usb_throughput (self):
        return self.usb_freq () * 4
        
    def set_waveform_type (self, type):
        '''
        valid waveform types are: gr.GR_SIN_WAVE, gr.GR_CONST_WAVE,
        gr.GR_UNIFORM and gr.GR_GAUSSIAN
        '''
        self._configure_graph (type)
        self.waveform_type = type

    def set_waveform_ampl (self, ampl):
        self.waveform_ampl = ampl
        self.siggen.set_amplitude (ampl)
        self.noisegen.set_amplitude (ampl)

    def set_waveform_freq (self, freq):
        self.waveform_freq = freq
        self.siggen.set_frequency (freq)
        
    def set_waveform_offset (self, offset):
        self.waveform_offset = offset
        self.siggen.set_offset (offset)

    def set_interpolator (self, interp):
        self.interp = interp
        self.siggen.set_sampling_freq (self.usb_freq ())
        self.u.set_interp_rate (interp)

    def _instantiate_blocks (self):
        self.src = None
        self.u = usrp.sink_c (0, self.interp)
        
        self.siggen = gr.sig_source_c (self.usb_freq (),
                                       gr.GR_SIN_WAVE,
                                       self.waveform_freq,
                                       self.waveform_ampl,
                                       self.waveform_offset)

        self.noisegen = gr.noise_source_c (gr.GR_UNIFORM,
                                           self.waveform_ampl)

        # self.file_sink = gr.file_sink (gr.sizeof_gr_complex, "siggen.dat")

    def _configure_graph (self, type):
        try:
            self.lock()
            self.disconnect_all ()
            if type == gr.GR_SIN_WAVE or type == gr.GR_CONST_WAVE:
                self.connect (self.siggen, self.u)
                # self.connect (self.siggen, self.file_sink)
                self.siggen.set_waveform (type)
                self.src = self.siggen
            elif type == gr.GR_UNIFORM or type == gr.GR_GAUSSIAN:
                self.connect (self.noisegen, self.u)
                self.noisegen.set_type (type)
                self.src = self.noisegen
            else:
                raise ValueError, type
        finally:
            self.unlock()

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
    parser.add_option ("-g", "--gain", type="eng_float", default=None,
                       help="set output gain to GAIN [default=%default]")
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

    tb = my_top_block()
    tb.set_interpolator (options.interp)
    tb.set_waveform_type (options.type)
    tb.set_waveform_freq (options.waveform_freq)
    tb.set_waveform_ampl (options.amplitude)
    tb.set_waveform_offset (options.offset)

    # determine the daughterboard subdevice we're using
    if options.tx_subdev_spec is None:
        options.tx_subdev_spec = usrp.pick_tx_subdevice(tb.u)

    m = usrp.determine_tx_mux_value(tb.u, options.tx_subdev_spec)
    #print "mux = %#04x" % (m,)
    tb.u.set_mux(m)
    tb.subdev = usrp.selected_subdev(tb.u, options.tx_subdev_spec)
    print "Using TX d'board %s" % (tb.subdev.side_and_name(),)
    
    if options.gain is None:
        tb.subdev.set_gain(tb.subdev.gain_range()[1])    # set max Tx gain
    else:
        tb.subdev.set_gain(options.gain)    # set max Tx gain

    if not tb.set_freq(options.rf_freq):
        sys.stderr.write('Failed to set RF frequency\n')
        raise SystemExit
    
    tb.subdev.set_enable(True)                       # enable transmitter

    try:
        tb.run()
    except KeyboardInterrupt:
        pass

if __name__ == '__main__':
    main ()

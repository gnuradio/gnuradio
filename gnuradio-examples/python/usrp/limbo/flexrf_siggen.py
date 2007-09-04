#!/usr/bin/env python

from gnuradio import gr, gru
from gnuradio import usrp
from gnuradio.eng_option import eng_option
from optparse import OptionParser
from gnuradio.wxgui import stdgui, slider
import wx

class flex_siggen (stdgui.gui_flow_graph):
    __slots__ = ['interp', 'waveform_type', 'waveform_ampl',
                 'waveform_freq', 'waveform_offset', 'fg', 'usrp',
                 'siggen', 'noisegen', 'src', 'file_sink' ]

    def __init__ (self,frame,panel,vbox,argv):
        stdgui.gui_flow_graph.__init__ (self, frame, panel, vbox, argv)
        
        self.frame = frame
        self.panel = panel

        parser = OptionParser (option_class=eng_option)
        parser.add_option ("-a", "--amplitude", type="int", default=32000,
                           help="amplitude")
        parser.add_option ("-i", "--interp", type="int", default=64,
                           help="set fpga interpolation rate to INTERP")
        parser.add_option ("-n", "--nchannels", type="int", default=1,
                           help="set number of output channels to NCHANNELS")
        (options, args) = parser.parse_args ()

        self.waveform_type = gr.GR_CONST_WAVE
        self.waveform_ampl = options.amplitude
        self.waveform_freq = 100.12345e3
        self.waveform_offset = 0

        self.interp = options.interp
        self._instantiate_blocks ()
        self.usrp.set_nchannels (options.nchannels)
        
        self.dboard=self.usrp.db[0][0]
        
        self.set_waveform_type (self.waveform_type)
        vbox.Add(slider.slider(panel, 390, 510, self.set_rf_freq), 1, wx.EXPAND|wx.ALIGN_CENTER)
        vbox.Add(slider.slider(panel, -45000, +45000, self.set_if_freq), 1, wx.EXPAND|wx.ALIGN_CENTER)
        #vbox.Add(slider.slider(panel, 0, 4095, self.set_gain), 1, wx.EXPAND|wx.ALIGN_CENTER)
        
    def usb_freq (self):
        return self.usrp.dac_freq() / self.interp

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
        
    def set_if_freq (self, freq):
        self.if_freq = freq
        self.usrp.set_tx_freq (0,freq*1e3)
        
    def set_rf_freq (self, freq):
        self.rf_freq = freq
        (success,actual_freq) = self.dboard.set_freq (freq*1e6)
        if not success:
            print "Failed on ", freq
            
    def set_waveform_offset (self, offset):
        self.waveform_offset = offset
        self.siggen.set_offset (offset)

    def set_interpolator (self, interp):
        self.interp = interp
        self.siggen.set_sampling_freq (self.usb_freq ())
        self.usrp.set_interp_rate (interp)

    def set_duc_freq (self, freq):
        self.usrp.set_tx_freq (0, freq)
        
    def _instantiate_blocks (self):
        self.src = None
        self.usrp = usrp.sink_c (0, self.interp)
        
        self.siggen = gr.sig_source_c (self.usb_freq (),
                                       gr.GR_SIN_WAVE,
                                       self.waveform_freq,
                                       self.waveform_ampl,
                                       self.waveform_offset)

        self.noisegen = gr.noise_source_c (gr.GR_UNIFORM,
                                           self.waveform_ampl)
        print "done"
        
    def _configure_graph (self, type):
        was_running = self.is_running ()
        if was_running:
            self.stop ()
        self.disconnect_all ()
        if type == gr.GR_SIN_WAVE or type == gr.GR_CONST_WAVE:
            self.connect (self.siggen, self.usrp)
            self.siggen.set_waveform (type)
            self.src = self.siggen
        elif type == gr.GR_UNIFORM or type == gr.GR_GAUSSIAN:
            self.connect (self.noisegen, self.usrp)
            self.noisegen.set_type (type)
            self.src = self.noisegen
        else:
            raise ValueError, type
        if was_running:
            self.start ()


if __name__ == '__main__':
    parser = OptionParser (option_class=eng_option)
    parser.add_option ("--sine", dest="type", action="store_const", const=gr.GR_SIN_WAVE,
                       help="generate a complex sinusoid [default]", default=gr.GR_SIN_WAVE)
    parser.add_option ("--const", dest="type", action="store_const", const=gr.GR_CONST_WAVE, 
                       help="generate a constant output")
    parser.add_option ("--gaussian", dest="type", action="store_const", const=gr.GR_GAUSSIAN,
                       help="generate Gaussian random output")
    parser.add_option ("--uniform", dest="type", action="store_const", const=gr.GR_UNIFORM,
                       help="generate Uniform random output")
    parser.add_option ("-f", "--freq", type="eng_float", default=100e3,
                       help="set waveform frequency to FREQ")
    parser.add_option ("-r", "--rf-freq", type="eng_float", default=910e6,
                       help="set waveform frequency to FREQ")
    parser.add_option ("-a", "--amplitude", type="eng_float", default=16e3,
                       help="set waveform amplitude to AMPLITUDE", metavar="AMPL")
    parser.add_option ("-o", "--offset", type="eng_float", default=0,
                       help="set waveform offset to OFFSET")
    parser.add_option ("-c", "--duc-freq", type="eng_float", default=0,
                       help="set Tx DUC frequency to FREQ", metavar="FREQ")
    parser.add_option ("-m", "--mux", type="intx", default=0x98,
                       help="set output mux register")
    
    app = stdgui.stdapp (flex_siggen, "USRP FlexRF Siggen")
    app.MainLoop ()

#!/usr/bin/env python

from gnuradio import gr, eng_notation
from gnuradio import audio
from gnuradio import usrp
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys
import math

from gnuradio.wxgui import stdgui, fftsink
import wx

class am_rx_graph (stdgui.gui_flow_graph):
    def __init__(self,frame,panel,vbox,argv):
        stdgui.gui_flow_graph.__init__ (self,frame,panel,vbox,argv)

        station = parseargs(argv[1:])
        offset_freq = 30e3
        IF_freq = offset_freq - station

        adc_rate = 64e6
        usrp_decim = 250
        if_rate = adc_rate / usrp_decim               # 256 kHz
        if_decim = 4
        demod_rate = if_rate / if_decim        # 64 kHz
        audio_decimation = 2
        audio_rate = demod_rate / audio_decimation  # 16 kHz
        
        # usrp is data source
        src = usrp.source_c (0, usrp_decim)
        src.set_rx_freq (0, IF_freq)
        actual_IF_freq =src.rx_freq(0)
        actual_offset = actual_IF_freq + station
        
        #print actual_IF_freq
        #print actual_offset
 
        src.set_pga(0,20)
        # sound card as final sink
        audio_sink = audio.sink (int (audio_rate))
        
        channel_coeffs = \
                       gr.firdes.low_pass (1.0,           # gain
                                           if_rate,   # sampling rate
                                           9e3,         # low pass cutoff freq
                                           10e3,         # width of trans. band
                                           gr.firdes.WIN_HANN)

        ddc =  gr.freq_xlating_fir_filter_ccf (if_decim,channel_coeffs,-actual_offset,if_rate)

        magblock = gr.complex_to_mag()
        volumecontrol = gr.multiply_const_ff(.003)

        # Deemphasis.  Is this necessary on AM?
        TAU  = 75e-6  # 75us in US, 50us in EUR
        fftaps = [ 1 - math.exp(-1/TAU/if_rate), 0]
        fbtaps= [ 0 , math.exp(-1/TAU/if_rate) ]
        
        deemph = gr.iir_filter_ffd(fftaps,fbtaps)

        # compute FIR filter taps for audio filter
        width_of_transition_band = audio_rate / 8
        audio_coeffs = gr.firdes.low_pass (1.0,            # gain
                                           if_rate,      # sampling rate
                                           9e3,         #audio_rate/2 - width_of_transition_band,
                                           4e3,         # width_of_transition_band,
                                           gr.firdes.WIN_HANN)
        
        # input: float; output: float
        audio_filter = gr.fir_filter_fff (audio_decimation, audio_coeffs)



        
        print len(channel_coeffs)
        print len(audio_coeffs)
        
        # now wire it all together
        self.connect (src, ddc)
        self.connect (ddc, magblock)
        self.connect (magblock, volumecontrol)
        self.connect (volumecontrol,deemph)
        self.connect (deemph,audio_filter)
        self.connect (audio_filter, (audio_sink, 0))

        if 1:
            pre_demod = fftsink.fft_sink_c (self, panel, title="Pre-Demodulation", fft_size=128, sample_rate=if_rate)
            self.connect (src, pre_demod)
            vbox.Add (pre_demod.win, 1, wx.EXPAND)

        if 0:
            post_demod = fftsink.fft_sink_c (self, panel, title="Post Demodulation", fft_size=256, sample_rate=demod_rate)
            self.connect (ddc, post_demod)
            vbox.Add (post_demod.win, 1, wx.EXPAND)

        if 0:
            post_filt = fftsink.fft_sink_f (self, panel, title="Post Filter", fft_size=512, sample_rate=audio_rate)
            self.connect (magblock,post_filt)
            vbox.Add (post_filt.win, 1, wx.EXPAND)
        
def parseargs (args):
    nargs = len (args)
    if nargs == 1:
        freq1 = float (args[0]) * 1e3
    else:
        sys.stderr.write ('usage: am_rcv freq1\n')
        sys.exit (1)

    return freq1

if __name__ == '__main__':
    app = stdgui.stdapp (am_rx_graph, "AM RX")
    app.MainLoop ()


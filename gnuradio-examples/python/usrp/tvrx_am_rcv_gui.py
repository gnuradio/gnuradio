#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 
#
# Demodulate an AM signal from the TVRX or a recorded file.
# The file format must be 256 ksps, complex data.
#

from gnuradio import gr, gru, eng_notation
from gnuradio import audio_oss as audio
from gnuradio import usrp
from gnuradio import tv_rx
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys
import math
from gnuradio.wxgui import stdgui, fftsink, scopesink
import wx

#
# return a gr.flow_graph
#
class wfm_rx_graph (stdgui.gui_flow_graph):
  def __init__(self,frame,panel,vbox,argv):
    stdgui.gui_flow_graph.__init__ (self,frame,panel,vbox,argv)
    
    #set rf freq
    rf_freq = 120.e6
    
    # Decimation rate from USRP ADC to IF.
    usrp_decim = 100
    
    # Calculate the sampling rate of the USRP and capture file.
    # Decimate the IF sampling rate down by 4 to 64 ksps
    # This is a flow graph that has an input (capture file) and output (audio channel).
    #self = gr.flow_graph ()
  
    # Signal source is assumed to be 256 kspb / complex data stream.
    which_side = 0
    # usrp is data source
    if which_side == 0:
        src = usrp.source_c (0, usrp_decim, 1, gru.hexint(0xf0f0f0f0), 0)
    else:
        src = usrp.source_c (0, usrp_decim, 1, gru.hexint(0xf0f0f0f2), 0)

    if_rate = 640e3 # src.adc_freq() / usrp_decim
    if_decim = 5
    demod_rate = if_rate / if_decim
    
    audio_decimation = 4
    audio_rate = demod_rate / audio_decimation

    # set up frontend
    dboard = tv_rx.tv_rx (src, which_side)
    self.dboard = dboard
    (success, actual_freq) = dboard.set_freq(rf_freq)
    assert success

    if_freq = rf_freq - actual_freq
    src.set_rx_freq (0, -if_freq)

    print "actual freq ", actual_freq
    print "IF freq ", if_freq
        
    dboard.set_gain(50)
    
    #src = gr.file_source (gr.sizeof_gr_complex, "samples/atis_ffz_am_baseband_256k_complex.dat")
    #src = gr.file_source (gr.sizeof_gr_complex, "samples/garagedoor1.dat", True)
    
    #channel_coeffs = gr.firdes.band_pass (
    #    1.0,    # gain
    #    if_rate,
    #    10,   # center of low transition band
    #    10000,   # center of hi transition band
    #    200,    # width of transition band
    #    gr.firdes.WIN_HAMMING)
    
    channel_coeffs = gr.firdes.low_pass (1.0, if_rate, 10e3, 4e3, gr.firdes.WIN_HANN)
    print "len(channel_coeffs) = ", len(channel_coeffs)

    # Tune to the desired frequency.
    ddc = gr.freq_xlating_fir_filter_ccf (if_decim, channel_coeffs, -20e3, if_rate)

    # Demodule with classic sqrt (I*I + Q*Q)
    magblock = gr.complex_to_mag()

    # Scale the audio
    volumecontrol = gr.multiply_const_ff(.1)

    #band-pass
    audio_coeffs = gr.firdes.band_pass (
        1.0,    # gain
        demod_rate,
        10,   # center of low transition band
        6000,   # center of hi transition band
        200,    # width of transition band
        gr.firdes.WIN_HAMMING)
    
    
    # Low pass filter the demodulator output
    #audio_coeffs = gr.firdes.low_pass (1.0, demod_rate, 500, 200, gr.firdes.WIN_HANN)
    print "len(audio_coeffs) = ", len(audio_coeffs)

    # input: float; output: float
    audio_filter = gr.fir_filter_fff (audio_decimation, audio_coeffs)

    # sound card as final sink
    audio_sink = audio.sink (int (audio_rate))

    # now wire it all together
    self.connect (src, ddc)
    self.connect (ddc, magblock)
    self.connect (magblock, volumecontrol)
    self.connect (volumecontrol, audio_filter)
    self.connect (audio_filter, (audio_sink, 0))
    
    d_win = fftsink.fft_sink_c (self, panel, title="RF", fft_size=512, sample_rate=if_rate)
    self.connect (src,d_win)
    vbox.Add (d_win.win, 4, wx.EXPAND)
       
    p_win = fftsink.fft_sink_c (self, panel, title="IF", fft_size=512, sample_rate=demod_rate)
    self.connect (ddc,p_win)
    vbox.Add (p_win.win, 4, wx.EXPAND)
       
    r_win = fftsink.fft_sink_f (self, panel, title="Audio", fft_size=512, sample_rate=audio_rate)
    self.connect (audio_filter,r_win)
    vbox.Add (r_win.win, 4, wx.EXPAND)
    
    #audio_oscope = scopesink.scope_sink_f (self, panel, "Oscope Data", audio_rate)
    #self.connect (audio_filter, audio_oscope)
    #vbox.Add (audio_oscope.win, 4, wx.EXPAND) 
                
if __name__ == '__main__':
    
    app = stdgui.stdapp (wfm_rx_graph, "TVRX AM RX")
    app.MainLoop ()

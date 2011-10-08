#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

# GUI interactions and high level connections handled here.
#
# Interacts with classes defined by wxGlade in ui.py.
#
# M. Revnell 2006-Jan

from threading import *
import wx
import wx.lib.evtmgr as em
import time

from gnuradio import gr, gru, eng_notation, optfir
from gnuradio import audio
from gnuradio import uhd
from gnuradio import blks2
from gnuradio.wxgui import fftsink2
from gnuradio.wxgui import waterfallsink2
from gnuradio.wxgui import scopesink2

from input    import *
from output   import *
from ssbdemod import *
from ssbagc   import *
from ui       import *
from math     import log10

class radio_top_block( gr.top_block ):
    def __init__( self ):
        gr.top_block.__init__(self, "radio_top_block")

        self.address      = "addr=192.168.11.2"
        self.samp_rate    = 256e3
        self.freq         = -2.5e6
        self.gain         = 0
        self.src          = uhd_input( self.address,
                                       self.samp_rate)
        self.src.set_freq(self.freq)
        self.src.set_gain(self.gain)

        self.fe_rate      = self.src.usrp_rate
        self.filter_decim = 1
        self.audio_decim  = 16
        self.demod_rate   = self.fe_rate / self.filter_decim
        self.audio_rate   = self.demod_rate / self.audio_decim
        self.audio_dev    = "pulse"

        self.demod        = ssb_demod( self.demod_rate, self.audio_rate )
        self.agc          = agc()
        self.out          = output( self.audio_rate, self.audio_dev )

        self.connect( self.src, self.demod, self.agc, self.out )

    def tune( self, freq ):
        fe_target = -freq
        self.src.set_freq( fe_target )
        demod_cf = fe_target - self.src.get_freq()
        self.demod.tune( demod_cf )

class radio_frame( ui_frame ):
    def __init__( self, block, *args, **kwds ):
        ui_frame.__init__( self, *args, **kwds )
        self.block = block
        self.freq_disp.SetRange(0, 30e6)
        f = self.block.src.freq
        self.freq_disp.SetValue( -f )
        self.volume.SetRange( 0, 20 )
        self.pga.SetRange( 0, 20 )
        self.rssi_range = 1
        self.rssi.SetRange( self.rssi_range )
        self.agc_max.SetValue( str( self.rssi_range ) )
        self.spin_e0.SetValue( 50 )
        self.spin_e1.SetValue( 50 )
        self.spin_e2.SetValue( 50 )
        self.spin_e3.SetValue( 50 )
        self.spin_e4.SetValue( 50 )
        self.spin_e5.SetValue( 50 )
        self.spin_e6.SetValue( 50 )
        bw = 3.3e3
        self.bandwidth.SetValue( str( bw ) )
        self.block.demod.set_bw( bw )
        self.bw_spin.SetValue( 5 )
        agc_gain = self.block.agc.gain.k()
        self.agc_gain_s.SetValue( 5 )
        self.agc_gain.SetValue( str( agc_gain ) )
        agc_ref = self.block.agc.offs.k()
        self.agc_ref.SetValue( str( agc_ref ) )
        self.agc_ref_s.SetValue( 5 )
        
        self.fespectrum = fftsink2.fft_sink_c(
            self.fe_panel,
            fft_size=512,
            sample_rate = block.fe_rate,
            ref_scale = 1.0,
            ref_level = 20.0,
            y_divs = 12,
            avg_alpha = 0.1)

        self.ifspectrum = fftsink2.fft_sink_c(
            self.if_panel,
            fft_size=512,
            sample_rate = block.audio_rate,
            ref_scale = 1.0,
            ref_level = 20.0,
            y_divs = 12,
            avg_alpha = 0.1)

        self.fespectrum.win.Bind( wx.EVT_MOTION, self.fe_mouse)
        self.fespectrum.win.Bind( wx.EVT_LEFT_DOWN, self.fe_click)

        block.connect( block.src.src, self.fespectrum )
        block.connect( block.demod.xlate, self.ifspectrum )

    def agc_ref_up( self, event ):
        self.agc_ref_s.SetValue( 5 )
        r = float( self.agc_ref.GetValue() )
        r = r + 5
        self.agc_ref.SetValue( str( r ) )
        self.block.agc.offs.set_k( r )
        
    def agc_ref_down( self, event ):
        self.agc_ref_s.SetValue( 5 )
        r = float( self.agc_ref.GetValue() )
        r = r - 5
        self.agc_ref.SetValue( str( r ) )
        self.block.agc.offs.set_k( r )

    def agc_gain_up( self, event ):
        self.agc_gain_s.SetValue( 5 )
        g = float(self.agc_gain.GetValue())
        g = g + 10
        self.agc_gain.SetValue( str( g ) )
        self.block.agc.gain.set_k( g )

    def agc_gain_down( self, event ):
        self.agc_gain_s.SetValue( 5 )
        g = float(self.agc_gain.GetValue())
        g = g - 10
        self.agc_gain.SetValue( str( g ) )
        self.block.agc.gain.set_k( g )

    def fe_mouse( self, event ):
        f = int(self.freq_disp.GetValue())
        f = f+((event.GetX()-346.)*(400./610.))*1000
        self.fespectrum.win.SetToolTip(
            wx.ToolTip( eng_notation.num_to_str(f)))

    def fe_click( self, event ):
        f = int(self.freq_disp.GetValue())
        f = f+((event.GetX()-346.)*(400./610.))*1000
        self.tune( f )

    def setrssi( self, level ):
        if level < 0:
            level = 0
        if level > self.rssi_range:
            self.rssi_range = level
            self.rssi.SetRange( level )
            self.agc_max.SetValue( str( level ))
        self.rssi.SetValue( level )
        self.agc_level.SetValue( str( level ))

    def tune_evt( self, event ):
        f = self.freq_disp.GetValue()
        self.tune( f )
        
    def tune( self, frequency ):
        self.freq_disp.SetValue( frequency )
        self.block.tune( frequency )

    def up_e0( self, event ):
        self.spin_e0.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() + 1e0 )

    def down_e0( self, event ):
        self.spin_e0.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() - 1e0 )

    def up_e1( self, event ):
        self.spin_e1.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() + 1e1 )

    def down_e1( self, event ):
        self.spin_e1.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() - 1e1 )

    def up_e2( self, event ):
        self.spin_e2.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() + 1e2 )

    def down_e2( self, event ):
        self.spin_e2.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() - 1e2 )

    def up_e3( self, event ):
        self.spin_e3.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() + 1e3 )

    def down_e3( self, event ):
        self.spin_e3.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() - 1e3 )

    def up_e4( self, event ):
        self.spin_e4.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() + 1e4 )

    def down_e4( self, event ):
        self.spin_e4.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() - 1e4 )

    def up_e5( self, event ):
        self.spin_e5.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() + 1e5 )

    def down_e5( self, event ):
        self.spin_e5.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() - 1e5 )

    def up_e6( self, event ):
        self.spin_e6.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() + 1e6 )

    def down_e6( self, event ):
        self.spin_e6.SetValue( 50 )
        self.tune( self.freq_disp.GetValue() - 1e6 )

    def event_pga( self, event ):
        self.block.src.set_gain(self.pga.GetValue())
        
    def event_vol( self, event ):
        self.block.out.set( self.volume.GetValue()/20.0 )

    def set_usb( self, event ):
        self.block.demod.upper_sb()
        
    def set_lsb( self, event ):
        self.block.demod.lower_sb()

    def set_am( self, event ):
        self.block.demod.set_am()

    def bw_up( self, event ):
        self.bw_spin.SetValue( 5 )
        bw = float(self.bandwidth.GetValue())
        bw = bw + 20.0
        if bw > 10e3:
            bw = 10e3
        self.bandwidth.SetValue( str( bw ) )
        self.block.demod.set_bw( bw )

    def bw_down( self, event ):
        self.bw_spin.SetValue( 5 )
        bw = float(self.bandwidth.GetValue())
        bw = bw - 20.0
        if bw < 50:
            bw = 50
        self.bandwidth.SetValue( str( bw ) )
        self.block.demod.set_bw( bw )


class radio( wx.App ):
    def OnInit( self ):
        self.block = radio_top_block()
        self.frame = radio_frame( self.block, None, -1, "HF Receiver" )
        self.frame.Show( True )
        self.SetTopWindow( self.frame )
        self.block.start()
        return True

def rssi_function():
    global radio_obj
    global sig_probe

    go = True
    while go:
        try:
            level = sig_probe.level()
            wx.CallAfter( radio_obj.frame.setrssi, level )
            time.sleep( .1 )
        except:
            go = False

def main():
    global radio_obj, sig_probe

    radio_obj = radio( 0 )
    sig_probe = gr.probe_signal_f()
    radio_obj.block.connect(radio_obj.block.agc.offs, sig_probe)

    thread2 = Thread( target = rssi_function )
    thread2.start()
      
    radio_obj.MainLoop()
    

if __name__ == "__main__":
    main()


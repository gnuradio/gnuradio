#!/usr/bin/env python

# GUI interactions and high level connections handled here.
#
# Interacts with classes defined by wxGlade in ui.py.
#
# The usual gnuradio copyright boilerplate incorperated here by reference.
#
# M. Revnell 2006-Jan

from threading import *
import wx
import wx.lib.evtmgr as em
import time

from gnuradio import gr, gru, eng_notation, optfir
from gnuradio import audio
from gnuradio import usrp
from gnuradio import blks
from gnuradio.wxgui import fftsink
from gnuradio.wxgui import waterfallsink
from gnuradio.wxgui import scopesink

from input    import *
from output   import *
from ssbdemod import *
from ssbagc   import *
from ui       import *
from math     import log10

class graph( gr.hier_block ):
    def __init__( self, fg ):
        self.graph        = fg
        self.fe_decim     = 250
        self.src          = input( self.fe_decim )
        self.adc_rate     = self.src.adc_rate
        self.fe_rate      = self.adc_rate / self.fe_decim
        self.filter_decim = 1
        self.audio_decim  = 16
        self.demod_rate   = self.fe_rate / self.filter_decim
        self.audio_rate   = self.demod_rate / self.audio_decim

        self.demod        = ssb_demod( fg, self.demod_rate, self.audio_rate )
        self.agc          = agc( fg )
        #self.agc          = gr.agc_ff()
        self.out          = output( fg, self.audio_rate )

        fg.connect( self.src.src,
                    self.demod,
                    self.agc,
                    self.out )

        gr.hier_block.__init__( self, fg, None, None )

    def tune( self, freq ):
        fe_target = -freq
        self.src.set_freq( fe_target )
        fe_freq = self.src.src.rx_freq( 0 )
        demod_cf = fe_target - fe_freq
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

        self.fespectrum = fftsink.fft_sink_c(
            self.block.graph,
            self.fe_panel,
            fft_size=512,
            sample_rate = block.fe_rate,
            baseband_freq = 0,
            average = False,
            size = ( 680, 140 ) )

        self.ifspectrum = fftsink.fft_sink_c(
            self.block.graph,
            self.if_panel,
            fft_size=512,
            sample_rate = block.audio_rate,
            baseband_freq = 0,
            average = False,
            size = ( 680, 140 ) )

        em.eventManager.Register( self.fe_mouse,
                                  wx.EVT_MOTION,
                                  self.fespectrum.win )

        em.eventManager.Register( self.fe_click,
                                  wx.EVT_LEFT_DOWN,
                                  self.fespectrum.win )

        block.graph.connect( block.src.src, self.fespectrum )
        block.graph.connect( block.demod.xlate, self.ifspectrum )

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
        self.block.src.src.set_pga( 0, self.pga.GetValue())
        
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
        self.graph = gr.flow_graph()
        self.block = graph( self.graph )
        self.frame = radio_frame( self.block, None, -1, "Title" )
        self.frame.Show( True )
        self.SetTopWindow( self.frame )
        return True

a=radio( 0 )

l=gr.probe_signal_f()
#l=gr.probe_avg_mag_sqrd_f(1,.001)
a.graph.connect(a.block.agc.offs,l )
#a.graph.connect(a.block.demod,l)

def main_function():
    global a
    a.MainLoop()


def rssi_function():
    global a
    global l
    while 1:
        level = l.level()
        wx.CallAfter( a.frame.setrssi, level )
        time.sleep( .1 )

thread1 = Thread( target = main_function )
thread2 = Thread( target = rssi_function )

thread1.start()
thread2.start()

a.graph.start()


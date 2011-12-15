#!/usr/bin/env python
#
# Copyright 2005-2007,2011 Free Software Foundation, Inc.
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

"""
Realtime capture and display of analog Tv stations.

Can also use a file as source or sink

When you use an output file you can show the results frame-by-frame
using ImageMagick

When you want to use the realtime sdl display window you must first
install gr-video-sdl.

When you use a file source, instead of the usrp, make sure you
capture interleaved shorts.  (Use usrp_rx_file.py, or use
usrp_rx_cfile.py --output-shorts if you have a recent enough
usrp_rx_cfile.py)

There is no synchronisation yet. The sync blocks are in development
but not yet in cvs.
"""

from gnuradio import gr
try:
  from gnuradio import video_sdl
except:
  print "FYI: gr-video-sdl is not installed"
  print "realtime SDL video output window will not be available"
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import slider, powermate
from gnuradio.wxgui import stdgui2, fftsink2, form
from optparse import OptionParser
import sys
import wx


class tv_rx_block (stdgui2.std_top_block):
    def __init__(self,frame,panel,vbox,argv):
        stdgui2.std_top_block.__init__ (self,frame,panel,vbox,argv)

        usage="%prog: [options] [input_filename]. \n If you don't specify an input filename the usrp will be used as source\n " \
              "Make sure your input capture file containes interleaved shorts not complex floats"
        parser=OptionParser(option_class=eng_option, usage=usage)
        parser.add_option("-a", "--args", type="string", default="",
                          help="UHD device address args [default=%default]")
        parser.add_option("", "--spec", type="string", default=None,
	                  help="Subdevice of UHD device where appropriate")
        parser.add_option("-A", "--antenna", type="string", default=None,
                          help="select Rx Antenna where appropriate")
        parser.add_option("-s", "--samp-rate", type="eng_float", default=1e6,
                          help="set sample rate")
        parser.add_option("-f", "--freq", type="eng_float", default=519.25e6,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-c", "--contrast", type="eng_float", default=1.0,
                          help="set contrast (default is 1.0)")
        parser.add_option("-b", "--brightness", type="eng_float", default=0.0,
                          help="set brightness (default is 0)")
        parser.add_option("-p", "--pal", action="store_true", default=False,
                          help="PAL video format (this is the default)")
        parser.add_option("-n", "--ntsc", action="store_true", default=False,
                          help="NTSC video format")
        parser.add_option("-o", "--out-filename", type="string", default="sdl",
                          help="For example out_raw_uchar.gray. If you don't specify an output filename you will get a video_sink_sdl realtime output window. You then need to have gr-video-sdl installed)")
        parser.add_option("-r", "--repeat", action="store_false", default=True,
                          help="repeat file in a loop")
        parser.add_option("", "--freq-min", type="eng_float", default=50.25e6,
                          help="Set a minimum frequency [default=%default]")
        parser.add_option("", "--freq-max", type="eng_float", default=900.25e6,
                          help="Set a maximum frequency [default=%default]")

        (options, args) = parser.parse_args()
        if not ((len(args) == 1) or (len(args) == 0)):
            parser.print_help()
            sys.exit(1)
        
        if len(args) == 1:
          filename = args[0]
        else:
          filename = None

        self.frame = frame
        self.panel = panel
        
        self.contrast = options.contrast
        self.brightness = options.brightness
        self.state = "FREQ"
        self.freq = 0

        self.tv_freq_min = options.freq_min
        self.tv_freq_max = options.freq_max

        # build graph
        self.u=None

        if not (options.out_filename=="sdl"):
          options.repeat=False

        usrp_rate = options.samp_rate

        if not ((filename is None) or (filename=="usrp")):
          # file is data source
          self.filesource = gr.file_source(gr.sizeof_short,filename,options.repeat)
          self.istoc = gr.interleaved_short_to_complex()
          self.connect(self.filesource,self.istoc)
          self.src=self.istoc

          options.gain=0.0
          self.gain=0.0

        else: # use a UHD device
          self.u = uhd.usrp_source(device_addr=options.args, stream_args=uhd.stream_args('fc32'))

          # Set the subdevice spec
          if(options.spec):
            self.u.set_subdev_spec(options.spec, 0)
            
          # Set the antenna
          if(options.antenna):
            self.u.set_antenna(options.antenna, 0)

          self.u.set_samp_rate(usrp_rate)
          dev_rate = self.u.get_samp_rate()

          if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.u.get_gain_range()
            options.gain = float(g.start()+g.stop())/2.0

          self.src=self.u

        self.gain = options.gain        

        f2uc=gr.float_to_uchar()

        # sdl window as final sink
        if not (options.pal or options.ntsc):
          options.pal=True #set default to PAL

        if options.pal:
          lines_per_frame=625.0
          frames_per_sec=25.0
          show_width=768

        elif options.ntsc:
          lines_per_frame=525.0
          frames_per_sec=29.97002997
          show_width=640

        width=int(usrp_rate/(lines_per_frame*frames_per_sec))
        height=int(lines_per_frame)

        if (options.out_filename=="sdl"):
          #Here comes the tv screen, you have to build and install
          #gr-video-sdl for this (subproject of gnuradio, only in cvs
          #for now)
          try:
            video_sink = video_sdl.sink_uc ( frames_per_sec, width, height, 0,
                                             show_width, height)
          except:
            print "gr-video-sdl is not installed"
            print "realtime \"sdl\" video output window is not available"
            raise SystemExit, 1
          self.dst=video_sink
        else:
          print "You can use the imagemagick display tool to show the resulting imagesequence"
          print "use the following line to show the demodulated TV-signal:"
          print "display -depth 8 -size " +str(width)+ "x" + str(height) \
              + " gray:" + options.out_filename
          print "(Use the spacebar to advance to next frames)" 
          options.repeat=False
          file_sink=gr.file_sink(gr.sizeof_char, options.out_filename)
          self.dst =file_sink 

        self.agc=gr.agc_cc(1e-7,1.0,1.0) #1e-7
        self.am_demod = gr.complex_to_mag ()
        self.set_blacklevel=gr.add_const_ff(0.0)
        self.invert_and_scale = gr.multiply_const_ff (0.0) #-self.contrast *128.0*255.0/(200.0)

        # now wire it all together
        #sample_rate=options.width*options.height*options.framerate

        process_type='do_no_sync'
        if process_type=='do_no_sync':
          self.connect (self.src, self.agc,self.am_demod,
                        self.invert_and_scale, self.set_blacklevel,
                        f2uc,self.dst)
        elif process_type=='do_tv_sync_adv':
          #defaults: gr.tv_sync_adv (double sampling_freq, unsigned
          #int tv_format,bool output_active_video_only=false, bool
          #do_invert=false, double wanted_black_level=0.0, double
          #wanted_white_level=255.0, double avg_alpha=0.1, double
          #initial_gain=1.0, double initial_offset=0.0,bool
          #debug=false)

          #note, this block is not yet in cvs
          self.tv_sync_adv=gr.tv_sync_adv(usrp_rate, 0, False, False,
                                          0.0, 255.0, 0.01, 1.0, 0.0, False)
          self.connect (self.src, self.am_demod, self.invert_and_scale,
                        self.tv_sync_adv, s2f, f2uc, self.dst) 

        elif process_type=='do_nullsink':
          #self.connect (self.src, self.am_demod,self.invert_and_scale,f2uc,video_sink)
          c2r=gr.complex_to_real()
          nullsink=gr.null_sink(gr.sizeof_float)
          self.connect (self.src, c2r,nullsink) #video_sink)
        elif process_type=='do_tv_sync_corr':
          frame_size=width*height #int(usrp_rate/25.0)
          nframes=10# 32
          search_window=20*nframes 
          debug=False
          video_alpha=0.3 #0.1
          corr_alpha=0.3
          
          #Note: this block is not yet in cvs
          tv_corr=gr.tv_correlator_ff(frame_size,nframes, search_window,
                                      video_alpha, corr_alpha,debug) 
          shift=gr.add_const_ff(-0.7)
          
          self.connect (self.src, self.agc, self.am_demod, tv_corr,
                        self.invert_and_scale, self.set_blacklevel,
                        f2uc, self.dst)
        else: # process_type=='do_test_image':
          src_vertical_bars = gr.sig_source_f (usrp_rate, gr.GR_SIN_WAVE,
                                               10.0 *usrp_rate/320, 255,128)
          self.connect(src_vertical_bars, f2uc, self.dst)

        self._build_gui(vbox, usrp_rate, usrp_rate, usrp_rate)
 

        frange = self.u.get_freq_range()
        if(frange.start() > self.tv_freq_max or frange.stop() <  self.tv_freq_min):
            sys.stderr.write("Radio does not support required frequency range.\n")
            sys.exit(1)
        if(options.freq < self.tv_freq_min or options.freq > self.tv_freq_max):
            sys.stderr.write("Requested frequency is outside of required frequency range.\n")
            sys.exit(1)

        # set initial values
        self.set_gain(options.gain)
        self.set_contrast(self.contrast)
        self.set_brightness(options.brightness)
        if not(self.set_freq(options.freq)):
            self._set_status_msg("Failed to set initial frequency")


    def _set_status_msg(self, msg, which=0):
        self.frame.GetStatusBar().SetStatusText(msg, which)


    def _build_gui(self, vbox, usrp_rate, demod_rate, audio_rate):

        def _form_set_freq(kv):
            return self.set_freq(kv['freq'])


        if 0:
            self.src_fft = fftsink.fft_sink_c (self, self.panel, title="Data from USRP",
                                               fft_size=512, sample_rate=usrp_rate)
            self.connect (self.src, self.src_fft)
            vbox.Add (self.src_fft.win, 4, wx.EXPAND)

        if 0:
            post_demod_fft = fftsink.fft_sink_f (self, self.panel, title="Post Demod",
                                                  fft_size=512, sample_rate=demod_rate,
                                                  y_per_div=10, ref_level=-40)
            self.connect (self.am_demod, post_demod_fft)
            vbox.Add (post_demod_fft.win, 4, wx.EXPAND)

        if 0:
            post_filt_fft = fftsink.fft_sink_f (self, self.panel, title="Post Filter", 
                                                fft_size=512, sample_rate=audio_rate,
                                                y_per_div=10, ref_level=-40)
            self.connect (self.set_blacklevel, post_filt)
            vbox.Add (fft_win4, 4, wx.EXPAND)

        
        # control area form at bottom
        self.myform = myform = form.form()

        if not (self.u is None):
          hbox = wx.BoxSizer(wx.HORIZONTAL)
          hbox.Add((5,0), 0)
          myform['freq'] = form.float_field(
            parent=self.panel, sizer=hbox, label="Freq", weight=1,
            callback=myform.check_input_and_call(_form_set_freq, self._set_status_msg))

          hbox.Add((5,0), 0)
          myform['freq_slider'] = \
              form.quantized_slider_field(parent=self.panel, sizer=hbox, weight=3,
                                        range=(self.tv_freq_min, self.tv_freq_max, 0.25e6),
                                        callback=self.set_freq)
          hbox.Add((5,0), 0)
          vbox.Add(hbox, 0, wx.EXPAND)

        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((5,0), 0)

        myform['contrast'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Contrast",
                                        weight=3, range=(-2.0, 2.0, 0.1),
                                        callback=self.set_contrast)
        hbox.Add((5,0), 1)

        myform['brightness'] = \
            form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Brightness",
                                        weight=3, range=(-255.0, 255.0, 1.0),
                                        callback=self.set_brightness)
        hbox.Add((5,0), 0)

        if not (self.u is None):
          g = self.u.get_gain_range()
          myform['gain'] = \
              form.quantized_slider_field(parent=self.panel, sizer=hbox, label="Gain",
                                        weight=3, range=(g.start(), g.stop(), g.step()),
                                        callback=self.set_gain)
          hbox.Add((5,0), 0)
        vbox.Add(hbox, 0, wx.EXPAND)

        try:
            self.knob = powermate.powermate(self.frame)
            self.rot = 0
            powermate.EVT_POWERMATE_ROTATE (self.frame, self.on_rotate)
            powermate.EVT_POWERMATE_BUTTON (self.frame, self.on_button)
        except:
            print "FYI: No Powermate or Contour Knob found"


    def on_rotate (self, event):
        self.rot += event.delta
        if (self.state == "FREQ"):
            if self.rot >= 3:
                self.set_freq(self.freq + .1e6)
                self.rot -= 3
            elif self.rot <=-3:
                self.set_freq(self.freq - .1e6)
                self.rot += 3
        elif (self.state == "CONTRAST"):
            step = 0.1
            if self.rot >= 3:
                self.set_contrast(self.contrast + step)
                self.rot -= 3
            elif self.rot <=-3:
                self.set_contrast(self.contrast - step)
                self.rot += 3
        else:
            step = 1
            if self.rot >= 3:
                self.set_brightness(self.brightness + step)
                self.rot -= 3
            elif self.rot <=-3:
                self.set_brightness(self.brightness - step)
                self.rot += 3
            
    def on_button (self, event):
        if event.value == 0:        # button up
            return
        self.rot = 0
        if self.state == "FREQ":
            self.state = "CONTRAST"
        elif self.state == "CONTRAST":
            self.state = "BRIGHTNESS"
        else:
            self.state = "FREQ"
        self.update_status_bar ()
        

    def set_contrast (self, contrast):
        self.contrast = contrast
        self.invert_and_scale.set_k(-self.contrast *128.0*255.0/(200.0))
        self.myform['contrast'].set_value(self.contrast)
        self.update_status_bar ()

    def set_brightness (self, brightness):
        self.brightness = brightness
        self.set_blacklevel.set_k(self.brightness +255.0)
        self.myform['brightness'].set_value(self.brightness)
        self.update_status_bar ()
                                        
    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital down converter.
        """
        if not (self.u is None):
          r = self.u.set_center_freq(target_freq)
          if r:
              self.freq = target_freq
              self.myform['freq'].set_value(target_freq)         # update displayed value
              self.myform['freq_slider'].set_value(target_freq)  # update displayed value
              self.update_status_bar()
              self._set_status_msg("OK", 0)
              return True

        self._set_status_msg("Failed", 0)
        return False

    def set_gain(self, gain):
      if not (self.u is None):
        self.gain=gain
        self.myform['gain'].set_value(gain)     # update displayed value
        self.u.set_gain(gain)
        self.update_status_bar()
        
    def update_status_bar (self):
      msg = "Setting:%s Contrast:%r Brightness:%r Gain: %r" % \
          (self.state, self.contrast,self.brightness,self.gain)
      self._set_status_msg(msg, 1)
        #self.src_fft.set_baseband_freq(self.freq)
        

if __name__ == '__main__':
    app = stdgui2.stdapp (tv_rx_block, "USRP TV RX black-and-white")
    app.MainLoop ()

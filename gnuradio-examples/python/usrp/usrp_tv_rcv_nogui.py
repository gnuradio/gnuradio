#!/usr/bin/env python
#
# Copyright 2005,2006,2007 Free Software Foundation, Inc.
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
Reads from a file and generates PAL TV pictures in black and white
which can be displayed using ImageMagick or realtime using gr-video-sdl
(To capture the input file Use usrp_rx_file.py, or use usrp_rx_cfile.py --output-shorts if you have a recent enough usrp_rx_cfile.py)
Can also use usrp directly as capture source, but then you need a higher decimation factor (64)
and thus get a lower horizontal resulution.
There is no synchronisation yet. The sync blocks are in development but not yet in cvs.

"""

from gnuradio import gr, eng_notation
from gnuradio import audio
from gnuradio import usrp
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys

try:
  from gnuradio import video_sdl
except:
  print "FYI: gr-video-sdl is not installed"
  print "realtime \"sdl\" video output window will not be available"


class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        usage="%prog: [options] output_filename. \n Special output_filename \"sdl\" will use video_sink_sdl as realtime output window. " \
              "You then need to have gr-video-sdl installed. \n" \
              "Make sure your input capture file containes interleaved shorts not complex floats"
        parser = OptionParser(option_class=eng_option, usage=usage)
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=(0, 0),
                          help="select USRP Rx side A or B (default=A)")
        parser.add_option("-c", "--contrast", type="eng_float", default=1.0,
                          help="set contrast (default is 1.0)")
        parser.add_option("-b", "--brightness", type="eng_float", default=0.0,
                          help="set brightness (default is 0)")
        parser.add_option("-d", "--decim", type="int", default=8,
                          help="set fgpa decimation rate to DECIM [default=%default]")
        parser.add_option("-i", "--in-filename", type="string", default=None,
                          help="Use input file as source. samples must be interleaved shorts \n " +
                               "Use usrp_rx_file.py or usrp_rx_cfile.py --output-shorts. \n"
                               "Special name \"usrp\"  results in realtime capturing and processing using usrp. \n" +
                               "You then probably need a decimation factor of 64 or higher.")
        parser.add_option("-f", "--freq", type="eng_float", default=None,
                          help="set frequency to FREQ.\nNote that the frequency of the video carrier is not at the middle of the TV channel", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-p", "--pal", action="store_true", default=False,
                          help="PAL video format (this is the default)")
        parser.add_option("-n", "--ntsc", action="store_true", default=False,
                          help="NTSC video format")
        parser.add_option("-r", "--repeat", action="store_false", default=True,
                          help="repeat in_file in a loop")
        parser.add_option("-8", "--width-8", action="store_true", default=False,
                          help="Enable 8-bit samples across USB")
        parser.add_option("-N", "--nframes", type="eng_float", default=None,
                          help="number of frames to collect [default=+inf]")
        parser.add_option( "--no-hb", action="store_true", default=False,
                          help="don't use halfband filter in usrp")
        (options, args) = parser.parse_args ()
        if not (len(args) == 1):
            parser.print_help()
            sys.stderr.write('You must specify the output. FILENAME or sdl \n');
            sys.exit(1)
        
        filename = args[0]

        if options.in_filename is None:
            parser.print_help()
            sys.stderr.write('You must specify the input -i FILENAME or -i usrp\n');
            raise SystemExit, 1

        if not (filename=="sdl"):
          options.repeat=False

        if not (options.in_filename=="usrp"):
          self.filesource = gr.file_source(gr.sizeof_short,options.in_filename,options.repeat) # file is data source, capture with usr_rx_csfile.py
          self.istoc = gr.interleaved_short_to_complex()
          self.connect(self.filesource,self.istoc)
          self.adc_rate=64e6
          self.src=self.istoc
        else:
          if options.freq is None:
            parser.print_help()
            sys.stderr.write('You must specify the frequency with -f FREQ\n');
            raise SystemExit, 1
          if abs(options.freq) < 1e6:
              options.freq *= 1e6
          if options.no_hb or (options.decim<8):
            self.fpga_filename="std_4rx_0tx.rbf" #contains 4 Rx paths without halfbands and 0 tx paths
          else:
            self.fpga_filename="std_2rxhb_2tx.rbf" # contains 2 Rx paths with halfband filters and 2 tx paths (the default)

          # build the graph
          self.u = usrp.source_c(decim_rate=options.decim,fpga_filename=self.fpga_filename)
          self.src=self.u
          if options.width_8:
              sample_width = 8
              sample_shift = 8
              format = self.u.make_format(sample_width, sample_shift)
              r = self.u.set_format(format)
          self.adc_rate=self.u.adc_freq()
          if options.rx_subdev_spec is None:
              options.rx_subdev_spec = usrp.pick_rx_subdevice(self.u)
          self.u.set_mux(usrp.determine_rx_mux_value(self.u, options.rx_subdev_spec))
          # determine the daughterboard subdevice we're using
          self.subdev = usrp.selected_subdev(self.u, options.rx_subdev_spec)
          print "Using RX d'board %s" % (self.subdev.side_and_name(),)

          if options.gain is None:
              # if no gain was specified, use the mid-point in dB
              g = self.subdev.gain_range()
              options.gain = float(g[0]+g[1])/2
          self.subdev.set_gain(options.gain)

          r = self.u.tune(0, self.subdev, options.freq)
          if not r:
              sys.stderr.write('Failed to set frequency\n')
              raise SystemExit, 1

        input_rate = self.adc_rate / options.decim
        print "video sample rate %s" % (eng_notation.num_to_str(input_rate))

        self.agc=gr.agc_cc(1e-7,1.0,1.0) #1e-7
        self.am_demod = gr.complex_to_mag ()
        self.set_blacklevel=gr.add_const_ff(options.brightness +255.0)
        self.invert_and_scale = gr.multiply_const_ff (-options.contrast *128.0*255.0/(200.0))
        self.f2uc=gr.float_to_uchar()

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
        width=int(input_rate/(lines_per_frame*frames_per_sec))
        height=int(lines_per_frame)

        if filename=="sdl":
          #Here comes the tv screen, you have to build and install gr-video-sdl for this (subproject of gnuradio, only in cvs for now)
          try:
            video_sink = video_sdl.sink_uc ( frames_per_sec, width, height,0,show_width,height)
          except:
            print "gr-video-sdl is not installed"
            print "realtime \"sdl\" video output window is not available"
            raise SystemExit, 1
          self.dst=video_sink
        else:
          print "You can use the imagemagick display tool to show the resulting imagesequence"
          print "use the following line to show the demodulated TV-signal:"
          print "display -depth 8 -size " +str(width)+ "x" + str(height) + " gray:" +filename
          print "(Use the spacebar to advance to next frames)" 
          file_sink=gr.file_sink(gr.sizeof_char, filename)
          self.dst =file_sink 

        if options.nframes is None:
            self.connect(self.src, self.agc)
        else:
            self.head = gr.head(gr.sizeof_gr_complex, int(options.nframes*width*height))
            self.connect(self.src, self.head, self.agc)

        self.connect (self.agc,self.am_demod,self.invert_and_scale, self.set_blacklevel,self.f2uc,self.dst)

        
if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass

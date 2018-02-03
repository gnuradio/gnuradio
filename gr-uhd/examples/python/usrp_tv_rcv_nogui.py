#!/usr/bin/env python
#
# Copyright 2005-2007,2011,2013 Free Software Foundation, Inc.
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
which can be displayed using ImageMagick or realtime using
gr-video-sdl (To capture the input file Use usrp_rx_file.py, or use
usrp_rx_cfile.py --output-shorts if you have a recent enough
usrp_rx_cfile.py)

Can also use usrp directly as capture source, but then you need a
higher decimation factor (64) and thus get a lower horizontal
resolution.  There is no synchronisation yet. The sync blocks are in
development but not yet in cvs.

"""

from gnuradio import gr, eng_notation
from gnuradio import analog
from gnuradio import blocks
from gnuradio import audio
from gnuradio import uhd
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

        usage=("%prog: [options] output_filename.\nSpecial output_filename" + \
            "\"sdl\" will use video_sink_sdl as realtime output window. " + \
            "You then need to have gr-video-sdl installed.\n" +\
            "Make sure your input capture file contains interleaved " + \
            "shorts not complex floats")
        parser = OptionParser(option_class=eng_option, usage=usage)
        parser.add_option("-a", "--args", type="string", default="",
                          help="UHD device address args [default=%default]")
        parser.add_option("", "--spec", type="string", default=None,
	                  help="Subdevice of UHD device where appropriate")
        parser.add_option("-A", "--antenna", type="string", default=None,
                          help="select Rx Antenna where appropriate")
        parser.add_option("-s", "--samp-rate", type="eng_float", default=1e6,
                          help="set sample rate")
        parser.add_option("-c", "--contrast", type="eng_float", default=1.0,
                          help="set contrast (default is 1.0)")
        parser.add_option("-b", "--brightness", type="eng_float", default=0.0,
                          help="set brightness (default is 0)")
        parser.add_option("-i", "--in-filename", type="string", default=None,
                          help="Use input file as source. samples must be " + \
                            "interleaved shorts \n Use usrp_rx_file.py or " + \
                            "usrp_rx_cfile.py --output-shorts.\n Special " + \
                            "name \"usrp\" results in realtime capturing " + \
                            "and processing using usrp.\n" + \
                            "You then probably need a decimation factor of 64 or higher.")
        parser.add_option("-f", "--freq", type="eng_float", default=519.25e6,
                          help="set frequency to FREQ.\nNote that the frequency of the video carrier is not at the middle of the TV channel", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-p", "--pal", action="store_true", default=False,
                          help="PAL video format (this is the default)")
        parser.add_option("-n", "--ntsc", action="store_true", default=False,
                          help="NTSC video format")
        parser.add_option("-r", "--repeat", action="store_false", default=True,
                          help="repeat in_file in a loop")
        parser.add_option("-N", "--nframes", type="eng_float", default=None,
                          help="number of frames to collect [default=+inf]")
        parser.add_option("", "--freq-min", type="eng_float", default=50.25e6,
                          help="Set a minimum frequency [default=%default]")
        parser.add_option("", "--freq-max", type="eng_float", default=900.25e6,
                          help="Set a maximum frequency [default=%default]")
        (options, args) = parser.parse_args ()
        if not (len(args) == 1):
            parser.print_help()
            sys.stderr.write('You must specify the output. FILENAME or sdl \n');
            sys.exit(1)

        filename = args[0]

        self.tv_freq_min = options.freq_min
        self.tv_freq_max = options.freq_max

        if options.in_filename is None:
            parser.print_help()
            sys.stderr.write('You must specify the input -i FILENAME or -i usrp\n');
            raise SystemExit, 1

        if not (filename=="sdl"):
          options.repeat=False

        input_rate = options.samp_rate
        print "video sample rate %s" % (eng_notation.num_to_str(input_rate))

        if not (options.in_filename=="usrp"):
          # file is data source, capture with usr_rx_csfile.py
          self.filesource = blocks.file_source(gr.sizeof_short,
                                               options.in_filename,
                                               options.repeat)
          self.istoc = blocks.interleaved_short_to_complex()
          self.connect(self.filesource,self.istoc)
          self.src=self.istoc
        else:
          if options.freq is None:
            parser.print_help()
            sys.stderr.write('You must specify the frequency with -f FREQ\n');
            raise SystemExit, 1

          # build the graph
          self.u = uhd.usrp_source(device_addr=options.args, stream_args=uhd.stream_args('fc32'))

          # Set the subdevice spec
          if(options.spec):
            self.u.set_subdev_spec(options.spec, 0)

          # Set the antenna
          if(options.antenna):
            self.u.set_antenna(options.antenna, 0)

          self.u.set_samp_rate(input_rate)
          dev_rate = self.u.get_samp_rate()

          self.src=self.u

          if options.gain is None:
              # if no gain was specified, use the mid-point in dB
              g = self.u.get_gain_range()
              options.gain = float(g.start()+g.stop())/2.0
          self.u.set_gain(options.gain)

          r = self.u.set_center_freq(options.freq)
          if not r:
              sys.stderr.write('Failed to set frequency\n')
              raise SystemExit, 1


        self.agc = analog.agc_cc(1e-7,1.0,1.0) #1e-7
        self.am_demod = blocks.complex_to_mag ()
        self.set_blacklevel = blocks.add_const_ff(options.brightness +255.0)
        self.invert_and_scale = blocks.multiply_const_ff(-options.contrast *128.0*255.0/(200.0))
        self.f2uc = blocks.float_to_uchar()

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
          #Here comes the tv screen, you have to build and install
          #gr-video-sdl for this (subproject of gnuradio)
          try:
            video_sink = video_sdl.sink_uc(frames_per_sec, width, height, 0,
                                           show_width,height)
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
          file_sink = blocks.file_sink(gr.sizeof_char, filename)
          self.dst =file_sink

        if options.nframes is None:
            self.connect(self.src, self.agc)
        else:
            self.head = blocks.head(gr.sizeof_gr_complex, int(options.nframes*width*height))
            self.connect(self.src, self.head, self.agc)

        self.connect (self.agc, self.am_demod, self.invert_and_scale,
                      self.set_blacklevel, self.f2uc, self.dst)

if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass

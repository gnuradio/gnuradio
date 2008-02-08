#!/usr/bin/env python
#
# Copyright 2006 Free Software Foundation, Inc.
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

from gnuradio import gr, gru
from gnuradio import audio
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import time
import sys

# Test script to test setting up the buffers using gr_test
# For very large buffers it will fail when you hit the circbuf memory limit.
# On linux this limit is shmmax, it will fail when it tries to create a buffer > shmmax.
# With a 2.6 or later kernel you can set the shmmax limit manually in a root console
#show current shmmax limit
#$ cat /proc/sys/kernel/shmmax
#33554432

#set shmmax limit manually to 300MB
#echo 300000000 >/proc/sys/kernel/shmmax

#show new shmmax limit
#$ cat /proc/sys/kernel/shmmax
#300000000

class my_graph(gr.top_block):

    def __init__(self, seconds,history,output_multiple):
        gr.top_block.__init__(self)

        parser = OptionParser(option_class=eng_option)
        parser.add_option("-O", "--audio-output", type="string", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option("-r", "--sample-rate", type="eng_float", default=48000,
                          help="set sample rate to RATE (48000)")
        (options, args) = parser.parse_args ()
        if len(args) != 0:
            parser.print_help()
            raise SystemExit, 1

        sample_rate = int(options.sample_rate)
        ampl = 0.1

        src0 = gr.sig_source_f (sample_rate, gr.GR_SIN_WAVE, 350, ampl)

        nsamples=int(sample_rate * seconds) #1 seconds
        # gr.test (const std::string &name=std::string("gr_test"),
        # int min_inputs=1, int max_inputs=1, unsigned int sizeof_input_item=1,
        # int min_outputs=1, int max_outputs=1, unsigned int sizeof_output_item=1,
        # unsigned int history=1,unsigned int output_multiple=1,double relative_rate=1.0,
        # bool fixed_rate=true,gr_consume_type_t cons_type=CONSUME_NOUTPUT_ITEMS, gr_produce_type_t prod_type=PRODUCE_NOUTPUT_ITEMS);
        name="gr_test"
        min_inputs=1
        max_inputs=1
        sizeof_input_item=gr.sizeof_float
        min_outputs=1
        max_outputs=1
        sizeof_output_item=gr.sizeof_float
        #history=1 # problems start at 8150
        #output_multiple=1 #problems start at 8000 in combination with large history
        relative_rate=1.0
        fixed_rate=True
        consume_type=gr.CONSUME_NOUTPUT_ITEMS
        produce_type=gr.PRODUCE_NOUTPUT_ITEMS
        test = gr.test(name, min_inputs,max_inputs,sizeof_input_item,
                                 min_outputs,max_outputs,sizeof_output_item,
                                 history,output_multiple,relative_rate,
                                 fixed_rate, consume_type,produce_type)
        #test = gr.test("gr_test",1,1,gr.sizeof_float,
        #                         1,1,gr.sizeof_float,
        #                         1,1,1.0,
        #                         True, gr.CONSUME_NOUTPUT_ITEMS,gr.PRODUCE_NOUTPUT_ITEMS)
        #unsigned int history=1,unsigned int output_multiple=1,double relative_rate=1.0,
        #bool fixed_rate=false
        dst = audio.sink (sample_rate, options.audio_output)
        head= gr.head(gr.sizeof_float, nsamples)

        self.connect (src0,test,head,(dst, 0))


if __name__ == '__main__':

    seconds=5.0
    output_multiple=1
    for history in (1,1000,8000,8100,8150,8175,8190,8191,8192,8193,8194,8195,9000,10000,100000,1000000,10000000): #,100000000):
      sys.stdout.flush()
      sys.stderr.flush()
      print 'Test with history=', history, 'output_multiple=',output_multiple
      sys.stdout.flush()
      sys.stderr.flush()
      succeed=True
      starttime=time.time()
      try:
          my_graph(seconds,history,output_multiple).run()
      except KeyboardInterrupt:
          pass
      except:
          print "\nAn exception has terminated the graph."
          exception=True
          succeed=False
      sys.stdout.flush()
      sys.stderr.flush()
      if succeed:
         print ''
      endtime=time.time()
      duration=endtime - starttime
      if (duration < 0.5*seconds) and (succeed):
         print "A problem has terminated the graph."
         succeed=False
      if (duration > 1.5*seconds) and (succeed):
         print "Something slowed the graph down considerably."
         succeed=False

      print 'The test result was:' , succeed
      print 'Test duration' , duration
      print ''

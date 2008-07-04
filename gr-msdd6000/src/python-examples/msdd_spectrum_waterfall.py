#!/usr/bin/env python
#
# Copyright 2008 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, eng_notation, optfir, window
from gnuradio import msdd
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys
import math
import struct
from pylab import *
from numpy import array
import time

matplotlib.interactive(True)
matplotlib.use('TkAgg')

class tune(gr.feval_dd):
    """
    This class allows C++ code to callback into python.
    """
    def __init__(self, tb):
        gr.feval_dd.__init__(self)
        self.tb = tb

    def eval(self, ignore):
        """
        This method is called from gr.bin_statistics_f when it wants to change
        the center frequency.  This method tunes the front end to the new center
        frequency, and returns the new frequency as its result.
        """
        try:
            # We use this try block so that if something goes wrong from here 
            # down, at least we'll have a prayer of knowing what went wrong.
            # Without this, you get a very mysterious:
            #
            #   terminate called after throwing an instance of 'Swig::DirectorMethodException'
            #   Aborted
            #
            # message on stderr.  Not exactly helpful ;)

            new_freq = self.tb.set_next_freq()
            return new_freq

        except Exception, e:
            print "tune: Exception: ", e


class parse_msg(object):
    def __init__(self, sample_rate, percent):
        self.axis_font_size = 16
        self.label_font_size = 18
        self.title_font_size = 20
        self.text_size = 22

        # Set up figures and subplots
        self.fig = figure(1, facecolor="w", figsize=(12,9))
        self.sp  = self.fig.add_subplot(1,1,1)
        self.pl  = self.sp.matshow(100*[range(100),])

        params = {'xtick.labelsize': self.axis_font_size,
                  'ytick.labelsize': self.axis_font_size}
        rcParams.update(params)

        # Throw up some title info
        self.sp.set_title(("FFT"), fontsize=self.title_font_size, fontweight="bold")
        self.sp.set_xlabel("Frequency (Hz)", fontsize=self.label_font_size, fontweight="bold")
        self.sp.set_ylabel("Sample index (should be time)", fontsize=self.label_font_size, fontweight="bold")

        self.freqrange = list()
        self.data = list()
        self.data3 = list()

        self.index = 0
        self.last_cfreq = 0

        # So we know how to splice the data
        self.sample_rate = sample_rate
        self.percent = (1.0-percent)/2.0
        
    def parse(self, msg):
        self.center_freq = msg.arg1()    # read the current center frequency
        self.vlen = int(msg.arg2())      # read the length of the data set received

        # wait until we wrap around before plotting the entire collected band
        if(self.center_freq < self.last_cfreq):
            #print "Plotting spectrum\n"

            # If we have 100 sets, start dropping the oldest
            if(len(self.data3) > 100):
                self.data3.pop(0)
            self.data3.append(self.data)

            # add the new data to the plot
            self.pl.set_data(self.data3)
            draw()

            # reset lists to collect next round
            self.index = 0
            del self.freqrange
            self.freqrange = list()
            del self.data
            self.data = list()
            #raw_input()

        self.last_cfreq = self.center_freq

        startind = int(self.percent * self.vlen)
        endind = int((1.0 - self.percent) * self.vlen)
        
        fstep = self.sample_rate / self.vlen
        f = [self.center_freq - self.sample_rate/2.0 + i*fstep for i in range(startind, endind)]
        self.freqrange += f

        t = msg.to_string();

        d = struct.unpack('%df' % (self.vlen,), t)

        self.data += [di for di in d[startind:endind]]
        

class my_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        # Build an options parser to bring in information from the user on usage
        usage = "usage: %prog [options] host min_freq max_freq"
        parser = OptionParser(option_class=eng_option, usage=usage)
        parser.add_option("-g", "--gain", type="eng_float", default=32,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("", "--tune-delay", type="eng_float", default=5e-5, metavar="SECS",
                          help="time to delay (in seconds) after changing frequency [default=%default]")
        parser.add_option("", "--dwell-delay", type="eng_float", default=50e-5, metavar="SECS",
                          help="time to dwell (in seconds) at a given frequncy [default=%default]")
        parser.add_option("-F", "--fft-size", type="int", default=256,
                          help="specify number of FFT bins [default=%default]")
        parser.add_option("-d", "--decim", type="intx", default=16,
                          help="set decimation to DECIM [default=%default]")
        parser.add_option("", "--real-time", action="store_true", default=False,
                          help="Attempt to enable real-time scheduling")

        (options, args) = parser.parse_args()
        if len(args) != 3:
            parser.print_help()
            sys.exit(1)

        # get user-provided info on address of MSDD and frequency to sweep
        self.address  = args[0]
        self.min_freq = eng_notation.str_to_num(args[1])
        self.max_freq = eng_notation.str_to_num(args[2])

        self.decim = options.decim
        self.gain  = options.gain
        
        if self.min_freq > self.max_freq:
            self.min_freq, self.max_freq = self.max_freq, self.min_freq   # swap them

	self.fft_size = options.fft_size

        if not options.real_time:
            realtime = False
        else:
            # Attempt to enable realtime scheduling
            r = gr.enable_realtime_scheduling()
            if r == gr.RT_OK:
                realtime = True
            else:
                realtime = False
                print "Note: failed to enable realtime scheduling"

        # Sampling rate is hardcoded and cannot be read off device
        adc_rate = 102.4e6
        self.int_rate = adc_rate / self.decim
        print "Sampling rate: ", self.int_rate

        # build graph
        self.port = 10001   # required port for UDP packets
	
	#	which board,	op mode,	adx,	port
#        self.src = msdd.source_c(0, 1, self.address, self.port)  # build source object

	self.conv = gr.interleaved_short_to_complex();

	self.src = msdd.source_simple(self.address,self.port);
        self.src.set_decim_rate(self.decim)                      # set decimation rate
#        self.src.set_desired_packet_size(0, 1460)                # set packet size to collect

        self.set_gain(self.gain)                                 # set receiver's attenuation
        self.set_freq(self.min_freq)                             # set receiver's rx frequency
        
        # restructure into vector format for FFT input
	s2v = gr.stream_to_vector(gr.sizeof_gr_complex, self.fft_size)

        # set up FFT processing block
        mywindow = window.blackmanharris(self.fft_size)
        fft = gr.fft_vcc(self.fft_size, True, mywindow, True)
        power = 0
        for tap in mywindow:
            power += tap*tap
        
        # calculate magnitude squared of output of FFT
        c2mag = gr.complex_to_mag_squared(self.fft_size)

        # FIXME the log10 primitive is dog slow
        log = gr.nlog10_ff(10, self.fft_size,
                           -20*math.log10(self.fft_size)-10*math.log10(power/self.fft_size))
		
        # Set the freq_step to % of the actual data throughput.
        # This allows us to discard the bins on both ends of the spectrum.
        self.percent = 0.4

        # Calculate the frequency steps to use in the collection over the whole bandwidth
        self.freq_step = self.percent * self.int_rate
        self.min_center_freq = self.min_freq + self.freq_step/2
        nsteps = math.ceil((self.max_freq - self.min_freq) / self.freq_step)
        self.max_center_freq = self.min_center_freq + (nsteps * self.freq_step)

        self.next_freq = self.min_center_freq
        
        # use these values to set receiver settling time between samples and sampling time
        # the default values provided seem to work well with the MSDD over 100 Mbps ethernet
        tune_delay  = max(0, int(round(options.tune_delay * self.int_rate / self.fft_size)))  # in fft_frames
        dwell_delay = max(1, int(round(options.dwell_delay * self.int_rate / self.fft_size))) # in fft_frames

        # set up message callback routine to get data from bin_statistics_f block
        self.msgq = gr.msg_queue(16)
        self._tune_callback = tune(self)        # hang on to this to keep it from being GC'd

        # FIXME this block doesn't like to work with negatives because of the "d_max[i]=0" on line
        # 151 of gr_bin_statistics_f.cc file. Set this to -10000 or something to get it to work.
        stats = gr.bin_statistics_f(self.fft_size, self.msgq,
                                    self._tune_callback, tune_delay, dwell_delay)

        # FIXME there's a concern over the speed of the log calculation
        # We can probably calculate the log inside the stats block
	self.connect(self.src, self.conv, s2v, fft, c2mag, log, stats)


    def set_next_freq(self):
        ''' Find and set the next frequency of the reciver. After going past the maximum frequency,
        the frequency is wrapped around to the start again'''
        target_freq = self.next_freq
        self.next_freq = self.next_freq + self.freq_step
        if self.next_freq >= self.max_center_freq:
            self.next_freq = self.min_center_freq

        if not self.set_freq(target_freq):
            print "Failed to set frequency to", target_freq

        return target_freq
                          

    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        """
        return self.src.set_rx_freq(0, target_freq)


    def set_gain(self, gain):
        self.src.set_pga(0, gain)


def main_loop(tb):
    # Set up parser to get data from stats block and display them.
    msgparser = parse_msg(tb.int_rate, tb.percent)
    
    while 1:
        # Get the next message sent from the C++ code (blocking call).
        # It contains the center frequency and the mag squared of the fft
	d = tb.msgq.delete_head();
	print d.to_string();
        msgparser.parse(d)
        #print msgparser.center_freq
    
if __name__ == '__main__':
    tb = my_top_block()
    try:
        tb.start()              # start executing flow graph in another thread...
        main_loop(tb)
        
    except KeyboardInterrupt:
        pass

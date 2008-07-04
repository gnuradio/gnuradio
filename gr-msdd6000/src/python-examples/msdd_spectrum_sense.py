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
    def __init__(self, sample_rate, percent, alpha=0.01):
        self.axis_font_size = 16
        self.label_font_size = 18
        self.title_font_size = 20
        self.text_size = 22

        self.fig = figure(1, facecolor="w", figsize=(12,9))
        self.sp  = self.fig.add_subplot(1,1,1)
        self.pl  = self.sp.plot(range(100), 100*[1,])

        params = {'backend': 'ps',
                  'xtick.labelsize': self.axis_font_size,
                  'ytick.labelsize': self.axis_font_size,
                  'text.usetex': False}
        rcParams.update(params)

        self.sp.set_title(("FFT"), fontsize=self.title_font_size, fontweight="bold")
        self.sp.set_xlabel("Frequency (Hz)", fontsize=self.label_font_size, fontweight="bold")
        self.sp.set_ylabel("Magnitude (dB)", fontsize=self.label_font_size, fontweight="bold")
        self.text_alpha = figtext(0.10, 0.94, ('Moving average alpha: %s' % alpha), weight="heavy", size=self.text_size)

        self.cfreqs = list()
        self.freqrange = list()
        self.data = list() #array('f')

        self.alpha = alpha

        self.index = 0
        self.full = False
        self.last_cfreq = 0
        
        self.sample_rate = sample_rate
        self.percent = (1.0-percent)/2.0
        
    def parse(self, msg):
        self.center_freq = msg.arg1()
        self.vlen = int(msg.arg2())
        assert(msg.length() == self.vlen * gr.sizeof_float)


        if(self.center_freq < self.last_cfreq):
            print "Plotting spectrum\n"
            self.full = True

            self.pl[0].set_data([self.freqrange, self.data])
            self.sp.set_ylim([min(self.data), max(self.data)])
            self.sp.set_xlim([min(self.freqrange), max(self.freqrange)])
            draw()

            self.index = 0
            del self.freqrange
            self.freqrange = list()
            #raw_input()

        self.last_cfreq = self.center_freq

        startind = int(self.percent * self.vlen)
        endind = int((1.0 - self.percent) * self.vlen)
        
        fstep = self.sample_rate / self.vlen
        f = [self.center_freq - self.sample_rate/2.0 + i*fstep for i in range(startind, endind)]
        self.freqrange += f

        t = msg.to_string()
        d = struct.unpack('%df' % (self.vlen,), t)

        if self.full:
            for i in range(startind, endind):
                self.data[self.index] = (1.0-self.alpha)*self.data[self.index] + (self.alpha)*d[i]
                self.index += 1
        else:
            self.data += [di for di in d[startind:endind]]
        

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        usage = "usage: %prog [options] host min_freq max_freq"
        parser = OptionParser(option_class=eng_option, usage=usage)
        parser.add_option("-g", "--gain", type="eng_float", default=None,
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

        adc_rate = 102.4e6
        self.int_rate = adc_rate / self.decim
        print "Sampling rate: ", self.int_rate

        # build graph
        self.port = 10001
        self.src = msdd.source_simple(self.address, self.port)
        self.src.set_decim_rate(self.decim)

        self.set_gain(self.gain)
        self.set_freq(self.min_freq)

	s2v = gr.stream_to_vector(gr.sizeof_gr_complex, self.fft_size)

        mywindow = window.blackmanharris(self.fft_size)
        fft = gr.fft_vcc(self.fft_size, True, mywindow, True)
        power = 0
        for tap in mywindow:
            power += tap*tap
        
        norm = gr.multiply_const_cc(1.0/self.fft_size)
        c2mag = gr.complex_to_mag_squared(self.fft_size)

        # FIXME the log10 primitive is dog slow
        log = gr.nlog10_ff(10, self.fft_size,
                           -20*math.log10(self.fft_size)-10*math.log10(power/self.fft_size))
		
        # Set the freq_step to % of the actual data throughput.
        # This allows us to discard the bins on both ends of the spectrum.
        self.percent = 0.4

        self.freq_step = self.percent * self.int_rate
        self.min_center_freq = self.min_freq + self.freq_step/2
        nsteps = math.ceil((self.max_freq - self.min_freq) / self.freq_step)
        self.max_center_freq = self.min_center_freq + (nsteps * self.freq_step)

        self.next_freq = self.min_center_freq
        
        tune_delay  = max(0, int(round(options.tune_delay * self.int_rate / self.fft_size)))  # in fft_frames
        dwell_delay = max(1, int(round(options.dwell_delay * self.int_rate / self.fft_size))) # in fft_frames

        self.msgq = gr.msg_queue(16)
        self._tune_callback = tune(self)        # hang on to this to keep it from being GC'd
        stats = gr.bin_statistics_f(self.fft_size, self.msgq,
                                    self._tune_callback, tune_delay, dwell_delay)

        # FIXME leave out the log10 until we speed it up
	self.connect(self.src, s2v, fft, c2mag, log, stats)


    def set_next_freq(self):
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
    msgparser = parse_msg(tb.int_rate, tb.percent)
    
    while 1:

        # Get the next message sent from the C++ code (blocking call).
        # It contains the center frequency and the mag squared of the fft
        msgparser.parse(tb.msgq.delete_head())

        # Print center freq so we know that something is happening...
        print msgparser.center_freq

        # FIXME do something useful with the data...
        
        # m.data are the mag_squared of the fft output (they are in the
        # standard order.  I.e., bin 0 == DC.)
        # You'll probably want to do the equivalent of "fftshift" on them
        # m.raw_data is a string that contains the binary floats.
        # You could write this as binary to a file.

    
if __name__ == '__main__':
    tb = my_top_block()
    try:
        tb.start()              # start executing flow graph in another thread...
        main_loop(tb)
        
    except KeyboardInterrupt:
        pass

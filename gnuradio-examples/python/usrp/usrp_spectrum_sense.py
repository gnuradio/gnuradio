#!/usr/bin/env python
#
# Copyright 2005,2007 Free Software Foundation, Inc.
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
from gnuradio import audio
from gnuradio import usrp
from gnuradio.eng_option import eng_option
from optparse import OptionParser
from usrpm import usrp_dbid
import sys
import math
import struct


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
    def __init__(self, msg):
        self.center_freq = msg.arg1()
        self.vlen = int(msg.arg2())
        assert(msg.length() == self.vlen * gr.sizeof_float)

        # FIXME consider using Numarray or NumPy vector
        t = msg.to_string()
        self.raw_data = t
        self.data = struct.unpack('%df' % (self.vlen,), t)


class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        usage = "usage: %prog [options] min_freq max_freq"
        parser = OptionParser(option_class=eng_option, usage=usage)
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=(0,0),
                          help="select USRP Rx side A or B (default=A)")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("", "--tune-delay", type="eng_float", default=1e-3, metavar="SECS",
                          help="time to delay (in seconds) after changing frequency [default=%default]")
        parser.add_option("", "--dwell-delay", type="eng_float", default=10e-3, metavar="SECS",
                          help="time to dwell (in seconds) at a given frequncy [default=%default]")
        parser.add_option("-F", "--fft-size", type="int", default=256,
                          help="specify number of FFT bins [default=%default]")
        parser.add_option("-d", "--decim", type="intx", default=16,
                          help="set decimation to DECIM [default=%default]")
        parser.add_option("", "--real-time", action="store_true", default=False,
                          help="Attempt to enable real-time scheduling")
        parser.add_option("-B", "--fusb-block-size", type="int", default=0,
                          help="specify fast usb block size [default=%default]")
        parser.add_option("-N", "--fusb-nblocks", type="int", default=0,
                          help="specify number of fast usb blocks [default=%default]")

        (options, args) = parser.parse_args()
        if len(args) != 2:
            parser.print_help()
            sys.exit(1)

        self.min_freq = eng_notation.str_to_num(args[0])
        self.max_freq = eng_notation.str_to_num(args[1])

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

        # If the user hasn't set the fusb_* parameters on the command line,
        # pick some values that will reduce latency.

        if 1:
            if options.fusb_block_size == 0 and options.fusb_nblocks == 0:
                if realtime:                        # be more aggressive
                    options.fusb_block_size = gr.prefs().get_long('fusb', 'rt_block_size', 1024)
                    options.fusb_nblocks    = gr.prefs().get_long('fusb', 'rt_nblocks', 16)
                else:
                    options.fusb_block_size = gr.prefs().get_long('fusb', 'block_size', 4096)
                    options.fusb_nblocks    = gr.prefs().get_long('fusb', 'nblocks', 16)
    
        #print "fusb_block_size =", options.fusb_block_size
	#print "fusb_nblocks    =", options.fusb_nblocks

        # build graph
        
        self.u = usrp.source_c(fusb_block_size=options.fusb_block_size,
                               fusb_nblocks=options.fusb_nblocks)


        adc_rate = self.u.adc_rate()                # 64 MS/s
        usrp_decim = options.decim
        self.u.set_decim_rate(usrp_decim)
        usrp_rate = adc_rate / usrp_decim

        self.u.set_mux(usrp.determine_rx_mux_value(self.u, options.rx_subdev_spec))
        self.subdev = usrp.selected_subdev(self.u, options.rx_subdev_spec)
        print "Using RX d'board %s" % (self.subdev.side_and_name(),)


	s2v = gr.stream_to_vector(gr.sizeof_gr_complex, self.fft_size)

        mywindow = window.blackmanharris(self.fft_size)
        fft = gr.fft_vcc(self.fft_size, True, mywindow)
        power = 0
        for tap in mywindow:
            power += tap*tap
            
        c2mag = gr.complex_to_mag_squared(self.fft_size)

        # FIXME the log10 primitive is dog slow
        log = gr.nlog10_ff(10, self.fft_size,
                           -20*math.log10(self.fft_size)-10*math.log10(power/self.fft_size))
		
        # Set the freq_step to 75% of the actual data throughput.
        # This allows us to discard the bins on both ends of the spectrum.

        self.freq_step = 0.75 * usrp_rate
        self.min_center_freq = self.min_freq + self.freq_step/2
        nsteps = math.ceil((self.max_freq - self.min_freq) / self.freq_step)
        self.max_center_freq = self.min_center_freq + (nsteps * self.freq_step)

        self.next_freq = self.min_center_freq
        
        tune_delay  = max(0, int(round(options.tune_delay * usrp_rate / self.fft_size)))  # in fft_frames
        dwell_delay = max(1, int(round(options.dwell_delay * usrp_rate / self.fft_size))) # in fft_frames

        self.msgq = gr.msg_queue(16)
        self._tune_callback = tune(self)        # hang on to this to keep it from being GC'd
        stats = gr.bin_statistics_f(self.fft_size, self.msgq,
                                    self._tune_callback, tune_delay, dwell_delay)

        # FIXME leave out the log10 until we speed it up
	#self.connect(self.u, s2v, fft, c2mag, log, stats)
	self.connect(self.u, s2v, fft, c2mag, stats)

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.subdev.gain_range()
            options.gain = float(g[0]+g[1])/2

        self.set_gain(options.gain)
	print "gain =", options.gain


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

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital down converter.
        """
        return self.u.tune(0, self.subdev, target_freq)


    def set_gain(self, gain):
        self.subdev.set_gain(gain)


def main_loop(tb):
    while 1:

        # Get the next message sent from the C++ code (blocking call).
        # It contains the center frequency and the mag squared of the fft
        m = parse_msg(tb.msgq.delete_head())

        # Print center freq so we know that something is happening...
        print m.center_freq

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

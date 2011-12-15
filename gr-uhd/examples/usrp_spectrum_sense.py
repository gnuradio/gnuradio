#!/usr/bin/env python
#
# Copyright 2005,2007,2011 Free Software Foundation, Inc.
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

from gnuradio import gr, eng_notation, window
from gnuradio import audio
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys
import math
import struct

sys.stderr.write("Warning: this is known to have issues on some machines+Python version combinations to seg fault due to the callback in bin_statitics. If you figure out why, we'd love to hear about it!\n")

class tune(gr.feval_dd):
    """
    This class allows C++ code to callback into python.
    """
    def __init__(self, tb):
        gr.feval_dd.__init__(self)
        self.tb = tb

    def eval(self, ignore):
        """
        This method is called from gr.bin_statistics_f when it wants
        to change the center frequency.  This method tunes the front
        end to the new center frequency, and returns the new frequency
        as its result.
        """

        try:
            # We use this try block so that if something goes wrong
            # from here down, at least we'll have a prayer of knowing
            # what went wrong.  Without this, you get a very
            # mysterious:
            #
            #   terminate called after throwing an instance of
            #   'Swig::DirectorMethodException' Aborted
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

        # FIXME consider using NumPy array
        t = msg.to_string()
        self.raw_data = t
        self.data = struct.unpack('%df' % (self.vlen,), t)


class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        usage = "usage: %prog [options] min_freq max_freq"
        parser = OptionParser(option_class=eng_option, usage=usage)
        parser.add_option("-a", "--args", type="string", default="",
                          help="UHD device device address args [default=%default]")
        parser.add_option("", "--spec", type="string", default=None,
	                  help="Subdevice of UHD device where appropriate")
        parser.add_option("-A", "--antenna", type="string", default=None,
                          help="select Rx Antenna where appropriate")
        parser.add_option("-s", "--samp-rate", type="eng_float", default=1e6,
                          help="set sample rate [default=%default]")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("", "--tune-delay", type="eng_float",
                          default=1e-3, metavar="SECS",
                          help="time to delay (in seconds) after changing frequency [default=%default]")
        parser.add_option("", "--dwell-delay", type="eng_float",
                          default=10e-3, metavar="SECS",
                          help="time to dwell (in seconds) at a given frequncy [default=%default]")
        parser.add_option("-F", "--fft-size", type="int", default=256,
                          help="specify number of FFT bins [default=%default]")
        parser.add_option("", "--real-time", action="store_true", default=False,
                          help="Attempt to enable real-time scheduling")

        (options, args) = parser.parse_args()
        if len(args) != 2:
            parser.print_help()
            sys.exit(1)

        self.min_freq = eng_notation.str_to_num(args[0])
        self.max_freq = eng_notation.str_to_num(args[1])

        if self.min_freq > self.max_freq:
            # swap them
            self.min_freq, self.max_freq = self.max_freq, self.min_freq

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

        # build graph
        self.u = uhd.usrp_source(device_addr=options.args, stream_args=uhd.stream_args('fc32'))

        # Set the subdevice spec
        if(options.spec):
            self.u.set_subdev_spec(options.spec, 0)

        # Set the antenna
        if(options.antenna):
            self.u.set_antenna(options.antenna, 0)

        usrp_rate = options.samp_rate
        self.u.set_samp_rate(usrp_rate)
        dev_rate = self.u.get_samp_rate()

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
                                    self._tune_callback, tune_delay,
                                    dwell_delay)

        # FIXME leave out the log10 until we speed it up
	#self.connect(self.u, s2v, fft, c2mag, log, stats)
	self.connect(self.u, s2v, fft, c2mag, stats)

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.u.get_gain_range()
            options.gain = float(g.start()+g.stop())/2.0

        self.set_gain(options.gain)
	print "gain =", options.gain

    def set_next_freq(self):
        target_freq = self.next_freq
        self.next_freq = self.next_freq + self.freq_step
        if self.next_freq >= self.max_center_freq:
            self.next_freq = self.min_center_freq

        if not self.set_freq(target_freq):
            print "Failed to set frequency to", target_freq
            sys.exit(1)

        return target_freq
                          

    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool
        """
        r = self.u.set_center_freq(target_freq)
        if r:
            return True

        return False

    def set_gain(self, gain):
        self.u.set_gain(gain)


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
        tb.start()
        main_loop(tb)
        
    except KeyboardInterrupt:
        pass

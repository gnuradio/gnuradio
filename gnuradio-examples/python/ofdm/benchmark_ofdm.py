#!/usr/bin/env python
#
# Copyright 2005, 2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

from gnuradio import gr, gru, modulation_utils
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import random, time, struct, sys, math, os

# from current dir
from transmit_path import transmit_path
from receive_path import receive_path
import ofdm

class awgn_channel(gr.hier_block):
    def __init__(self, fg, sample_rate, noise_voltage, frequency_offset):

        self.input = gr.add_const_cc(0)

        self.noise_adder = gr.add_cc()
        self.noise = gr.noise_source_c(gr.GR_GAUSSIAN,noise_voltage)
        self.offset = gr.sig_source_c(1, gr.GR_SIN_WAVE, frequency_offset, 1.0, 0.0)
        self.mixer_offset = gr.multiply_cc()

        fg.connect(self.input, (self.mixer_offset,0))
        fg.connect(self.offset,(self.mixer_offset,1))
        fg.connect(self.mixer_offset, (self.noise_adder,1))
        fg.connect(self.noise, (self.noise_adder,0))

        gr.hier_block.__init__(self, fg, self.input, self.noise_adder)

class multipath_channel(gr.hier_block):
    def __init__(self, fg):

        self.taps = [1.0, .2, 0.0, .1, .08, -.4, .12, -.2, 0, 0, 0, .3]
        self.chan = gr.fir_filter_ccc(1, self.taps)
        
        gr.hier_block.__init__(self, fg, self.chan, self.chan)

class my_graph(gr.flow_graph):
    def __init__(self, callback, options):
        gr.flow_graph.__init__(self)

        channel_on = True

        SNR = 10.0**(options.snr/10.0)
        frequency_offset = options.frequency_offset / options.fft_length
        
        power_in_signal = options.occupied_tones
        noise_power_in_channel = power_in_signal/SNR
        noise_power_required = noise_power_in_channel * options.fft_length / options.occupied_tones
        noise_voltage = math.sqrt(noise_power_required)

        self.txpath = transmit_path(self, options)
        self.throttle = gr.throttle(gr.sizeof_gr_complex, options.sample_rate)
        self.rxpath = receive_path(self, callback, options)

        if channel_on:
            self.channel = awgn_channel(self, options.sample_rate, noise_voltage, frequency_offset)
            self.multipath = multipath_channel(self)

            if options.discontinuous:
                z = 20000*[0,]
                self.zeros = gr.vector_source_c(z, True)
                packet_size = 15*((4+8+4+1500+4) * 8)
                self.mux = gr.stream_mux(gr.sizeof_gr_complex, [packet_size-0, int(10e5)])

                # Connect components
                self.connect(self.txpath, (self.mux,0))
                self.connect(self.zeros, (self.mux,1))
                self.connect(self.mux, self.throttle, self.channel, self.rxpath)
                self.connect(self.mux, gr.file_sink(gr.sizeof_gr_complex, "tx_ofdm.dat"))

            else:
                #self.connect(self.txpath, self.throttle, self.multipath, self.channel)
                self.connect(self.txpath, self.throttle, self.channel)
                self.connect(self.channel, self.rxpath)
                self.connect(self.txpath, gr.file_sink(gr.sizeof_gr_complex, "tx_ofdm.dat"))
            
        else:
            self.connect(self.txpath, self.throttle, self.rxpath)
            self.connect(self.txpath, gr.file_sink(gr.sizeof_gr_complex, "tx"))
            self.connect(self.rxpath.ofdm_demod.ofdm_rx, gr.file_sink(options.fft_length*gr.sizeof_gr_complex, "rx"))


# /////////////////////////////////////////////////////////////////////////////
#                                   main
# /////////////////////////////////////////////////////////////////////////////

def main():
    global n_rcvd, n_right
        
    n_rcvd = 0
    n_right = 0
        
    def send_pkt(payload='', eof=False):
        return fg.txpath.send_pkt(payload, eof)
        
    def rx_callback(ok, payload):
        global n_rcvd, n_right
        n_rcvd += 1
        (pktno,) = struct.unpack('!H', payload[0:2])
        if ok:
            n_right += 1
        print "ok: %r \t pktno: %d \t n_rcvd: %d \t n_right: %d" % (ok, pktno, n_rcvd, n_right)
                
    parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
    expert_grp = parser.add_option_group("Expert")
    parser.add_option("-s", "--size", type="eng_float", default=1450,
                      help="set packet size [default=%default]")
    parser.add_option("-M", "--megabytes", type="eng_float", default=1.0,
                      help="set megabytes to transmit [default=%default]")
    parser.add_option("-r", "--sample-rate", type="eng_float", default=1e5,
                      help="set sample rate to RATE (%default)") 
    parser.add_option("", "--snr", type="eng_float", default=30,
                      help="set the SNR of the channel in dB [default=%default]")
    parser.add_option("", "--frequency-offset", type="eng_float", default=0,
                      help="set frequency offset introduced by channel [default=%default]")
    parser.add_option("","--discontinuous", action="store_true", default=False,
                      help="enable discontinous transmission (bursts of 5 packets)")

    transmit_path.add_options(parser, expert_grp)
    receive_path.add_options(parser, expert_grp)
    ofdm.ofdm_mod.add_options(parser, expert_grp)
    ofdm.ofdm_demod.add_options(parser, expert_grp)
    
    (options, args) = parser.parse_args ()

    if(options.mtu < options.size):
        sys.stderr.write("MTU (%.0f) must be larger than the packet size (%.0f)\n"
                         % (options.mtu, options.size))
        sys.exit(1)
        
    # build the graph
    fg = my_graph(rx_callback, options)
    
    r = gr.enable_realtime_scheduling()
    #    if r != gr.RT_OK:
    #        print "Warning: failed to enable realtime scheduling"
        
    fg.start()                       # start flow graph
    
    # generate and send packets
    nbytes = int(1e6 * options.megabytes)
    n = 0
    pktno = 0
    pkt_size = int(options.size)

    
    
    while n < nbytes:
        r = ''.join([chr(random.randint(0,255)) for i in range(pkt_size-2)])
        #pkt_contents = struct.pack('!H', pktno) + (pkt_size - 2) * chr(pktno & 0xff)
        pkt_contents = struct.pack('!H', pktno) + r
        send_pkt(pkt_contents)
        n += pkt_size
        #sys.stderr.write('.')
        #if options.discontinuous and pktno % 5 == 4:
        #    time.sleep(1)
        pktno += 1
        
    send_pkt(eof=True)
    fg.wait()                       # wait for it to finish


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass



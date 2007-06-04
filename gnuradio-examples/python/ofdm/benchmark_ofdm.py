#!/usr/bin/env python
#
# Copyright 2006, 2007 Free Software Foundation, Inc.
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

from gnuradio import gr, blks
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
from gnuradio.blksimpl import ofdm_pkt

import random, time, struct, sys, math, os

# from current dir
from transmit_path import transmit_path
from receive_path import receive_path


class my_graph(gr.flow_graph):
    def __init__(self, callback, options):
        gr.flow_graph.__init__(self)

        if options.channel_on:
            SNR = 10.0**(options.snr/10.0)
            power_in_signal = 1.0
            noise_power_in_channel = power_in_signal/SNR
            noise_voltage = math.sqrt(noise_power_in_channel/2.0)
            print "Noise voltage: ", noise_voltage

            frequency_offset = options.frequency_offset / options.fft_length

            if options.multipath_on:
                taps = [1.0, .2, 0.0, .1, .08, -.4, .12, -.2, 0, 0, 0, .3]
            else:
                taps = [1.0, 0.0]

        else:
            noise_voltage = 0.0
            frequency_offset = 0.0
            taps = [1.0, 0.0]

        symbols_per_packet = math.ceil(((4+options.size+4) * 8) / options.occupied_tones)
        samples_per_packet = (symbols_per_packet+2) * (options.fft_length+options.cp_length)
        print "Symbols per Packet: ", symbols_per_packet
        print "Samples per Packet: ", samples_per_packet
        if options.discontinuous:
            stream_size = [100000, int(options.discontinuous*samples_per_packet)]
        else:
            stream_size = [0, 100000]

        z = [0,]
        self.zeros = gr.vector_source_c(z, True)
        self.txpath = transmit_path(self, options)

        self.mux = gr.stream_mux(gr.sizeof_gr_complex, stream_size)
        self.throttle = gr.throttle(gr.sizeof_gr_complex, options.sample_rate)
        self.channel = blks.channel_model(self, noise_voltage, frequency_offset, options.clockrate_ratio, taps)
        self.rxpath = receive_path(self, callback, options)
                
        self.connect(self.zeros, (self.mux,0))
        self.connect(self.txpath, (self.mux,1))
        self.connect(self.mux, self.throttle, self.channel, self.rxpath)
        
        self.connect(self.txpath, gr.file_sink(gr.sizeof_gr_complex, "txpath.dat"))
        self.connect(self.mux, gr.file_sink(gr.sizeof_gr_complex, "mux.dat"))
        self.connect(self.channel, gr.file_sink(gr.sizeof_gr_complex, "channel.dat"))
            
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

        printlst = list()
        for x in payload[2:]:
            t = hex(ord(x)).replace('0x', '')
            if(len(t) == 1):
                t = '0' + t
            printlst.append(t)
        printable = ''.join(printlst)

        print printable
        print "\n"
                
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
    parser.add_option("", "--clockrate-ratio", type="eng_float", default=1.0,
                      help="set clock rate ratio (sample rate difference) between two systems [default=%default]")
    parser.add_option("","--discontinuous", type="int", default=0,
                      help="enable discontinous transmission, burst of N packets [Default is continuous]")
    parser.add_option("","--channel-on", action="store_true", default=True,
                      help="Enables AWGN, freq offset")
    parser.add_option("","--multipath-on", action="store_true", default=False,
                      help="enable multipath")

    transmit_path.add_options(parser, expert_grp)
    receive_path.add_options(parser, expert_grp)
    ofdm_pkt.mod_ofdm_pkts.add_options(parser, expert_grp)
    ofdm_pkt.demod_ofdm_pkts.add_options(parser, expert_grp)
    
    (options, args) = parser.parse_args ()
       
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
        #r = ''.join([chr(random.randint(0,255)) for i in range(pkt_size-2)])
        #pkt_contents = struct.pack('!H', pktno) + r

        pkt_contents = struct.pack('!H', pktno) + (pkt_size - 2) * chr(pktno & 0xff)

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



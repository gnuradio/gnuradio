#!/usr/bin/env python
#!/usr/bin/env python
#
# Copyright 2005, 2006,2007 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, modulation_utils
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import random, time, struct, sys, math

# from current dir
from transmit_path_lb import transmit_path
from receive_path_lb import receive_path
import fusb_options

class awgn_channel(gr.hier_block2):
    def __init__(self, sample_rate, noise_voltage, frequency_offset, seed=False):
        gr.hier_block2.__init__(self, "awgn_channel",
                                gr.io_signature(1,1,gr.sizeof_gr_complex), # Input signature
                                gr.io_signature(1,1,gr.sizeof_gr_complex)) # Output signature

        # Create the Gaussian noise source
        if not seed:
            self.noise = gr.noise_source_c(gr.GR_GAUSSIAN, noise_voltage)
        else:
            rseed = int(time.time())
            self.noise = gr.noise_source_c(gr.GR_GAUSSIAN, noise_voltage, rseed)
        self.adder = gr.add_cc()

        # Create the frequency offset
        self.offset = gr.sig_source_c((sample_rate*1.0), gr.GR_SIN_WAVE, frequency_offset, 1.0, 0.0)
        self.mixer = gr.multiply_cc()

        # Connect the components
        self.connect(self,        (self.mixer, 0))
        self.connect(self.offset, (self.mixer, 1))
        self.connect(self.mixer,  (self.adder, 0))
        self.connect(self.noise,  (self.adder, 1))
        self.connect(self.adder,   self)


class my_graph(gr.top_block):
    def __init__(self, mod_class, demod_class, rx_callback, options):
        gr.top_block.__init__(self, "my_graph")

        channelon = True;

        SNR = 10.0**(options.snr/10.0)
        frequency_offset = options.frequency_offset
        
        power_in_signal = abs(options.tx_amplitude)**2
        noise_power = power_in_signal/SNR
        noise_voltage = math.sqrt(noise_power)

        self.txpath = transmit_path(mod_class, options)
        self.throttle = gr.throttle(gr.sizeof_gr_complex, options.sample_rate)
        self.rxpath = receive_path(demod_class, rx_callback, options)

        if channelon:
            self.channel = awgn_channel(options.sample_rate, noise_voltage, frequency_offset, options.seed)
            self.connect(self.txpath, self.throttle, self.channel, self.rxpath)
        else:
            self.connect(self.txpath, self.throttle, self.rxpath)

# /////////////////////////////////////////////////////////////////////////////
#                                   main
# /////////////////////////////////////////////////////////////////////////////

def main():

    global n_rcvd, n_right

    n_rcvd = 0
    n_right = 0
    
    def rx_callback(ok, payload):
        global n_rcvd, n_right
        (pktno,) = struct.unpack('!H', payload[0:2])
        n_rcvd += 1
        if ok:
            n_right += 1

        print "ok = %5s  pktno = %4d  n_rcvd = %4d  n_right = %4d" % (
            ok, pktno, n_rcvd, n_right)

    def send_pkt(payload='', eof=False):
        return top_block.txpath.send_pkt(payload, eof)


    mods = modulation_utils.type_1_mods()
    demods = modulation_utils.type_1_demods()

    parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
    expert_grp = parser.add_option_group("Expert")
    channel_grp = parser.add_option_group("Channel")

    parser.add_option("-m", "--modulation", type="choice", choices=mods.keys(),
                      default='dbpsk',
                      help="Select modulation from: %s [default=%%default]"
                            % (', '.join(mods.keys()),))

    parser.add_option("-s", "--size", type="eng_float", default=1500,
                      help="set packet size [default=%default]")
    parser.add_option("-M", "--megabytes", type="eng_float", default=1.0,
                      help="set megabytes to transmit [default=%default]")
    parser.add_option("","--discontinuous", action="store_true", default=False,
                      help="enable discontinous transmission (bursts of 5 packets)")

    channel_grp.add_option("", "--sample-rate", type="eng_float", default=1e5,
                           help="set speed of channel/simulation rate to RATE [default=%default]") 
    channel_grp.add_option("", "--snr", type="eng_float", default=30,
                           help="set the SNR of the channel in dB [default=%default]")
    channel_grp.add_option("", "--frequency-offset", type="eng_float", default=0,
                           help="set frequency offset introduced by channel [default=%default]")
    channel_grp.add_option("", "--seed", action="store_true", default=False,
                           help="use a random seed for AWGN noise [default=%default]")

    transmit_path.add_options(parser, expert_grp)
    receive_path.add_options(parser, expert_grp)

    for mod in mods.values():
        mod.add_options(expert_grp)
    for demod in demods.values():
        demod.add_options(expert_grp)

    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help()
        sys.exit(1)
 
    r = gr.enable_realtime_scheduling()
    if r != gr.RT_OK:
        print "Warning: failed to enable realtime scheduling"
        
    # Create an instance of a hierarchical block
    top_block = my_graph(mods[options.modulation], demods[options.modulation], rx_callback, options)
    top_block.start()

    # generate and send packets
    nbytes = int(1e6 * options.megabytes)
    n = 0
    pktno = 0
    pkt_size = int(options.size)

    while n < nbytes:
        send_pkt(struct.pack('!H', pktno) + (pkt_size - 2) * chr(pktno & 0xff))
        n += pkt_size
        if options.discontinuous and pktno % 5 == 4:
            time.sleep(1)
        pktno += 1
        
    send_pkt(eof=True)

    top_block.wait()
    
if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass

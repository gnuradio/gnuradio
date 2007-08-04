#!/usr/bin/env python
#
# Copyright 2005,2006 Free Software Foundation, Inc.
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


# /////////////////////////////////////////////////////////////////////////////
#
#    This code sets up up a virtual ethernet interface (typically gr0),
#    and relays packets between the interface and the GNU Radio PHY+MAC
#
#    What this means in plain language, is that if you've got a couple
#    of USRPs on different machines, and if you run this code on those
#    machines, you can talk between them using normal TCP/IP networking.
#
# /////////////////////////////////////////////////////////////////////////////


from gnuradio import gr, gru, blks
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import random
import time
import struct
import sys
import os

# from current dir
from transmit_path import transmit_path
from receive_path import receive_path
import fusb_options

#print os.getpid()
#raw_input('Attach and press enter')


# /////////////////////////////////////////////////////////////////////////////
#
#   Use the Universal TUN/TAP device driver to move packets to/from kernel
#
#   See /usr/src/linux/Documentation/networking/tuntap.txt
#
# /////////////////////////////////////////////////////////////////////////////

# Linux specific...
# TUNSETIFF ifr flags from <linux/tun_if.h>

IFF_TUN		= 0x0001   # tunnel IP packets
IFF_TAP		= 0x0002   # tunnel ethernet frames
IFF_NO_PI	= 0x1000   # don't pass extra packet info
IFF_ONE_QUEUE	= 0x2000   # beats me ;)

def open_tun_interface(tun_device_filename):
    from fcntl import ioctl
    
    mode = IFF_TAP | IFF_NO_PI
    TUNSETIFF = 0x400454ca

    tun = os.open(tun_device_filename, os.O_RDWR)
    ifs = ioctl(tun, TUNSETIFF, struct.pack("16sH", "gr%d", mode))
    ifname = ifs[:16].strip("\x00")
    return (tun, ifname)
    

# /////////////////////////////////////////////////////////////////////////////
#                             the flow graph
# /////////////////////////////////////////////////////////////////////////////

class usrp_graph(gr.flow_graph):
    def __init__(self, callback, options):
        gr.flow_graph.__init__(self)

        self._tx_freq            = options.tx_freq         # tranmitter's center frequency
        self._tx_subdev_spec     = options.tx_subdev_spec  # daughterboard to use
        self._interp             = options.interp          # interpolating rate for the USRP (prelim)
        self._rx_freq            = options.rx_freq         # receiver's center frequency
        self._rx_gain            = options.rx_gain         # receiver's gain
        self._rx_subdev_spec     = options.rx_subdev_spec  # daughterboard to use
        self._decim              = options.decim           # Decimating rate for the USRP (prelim)
        self._fusb_block_size    = options.fusb_block_size # usb info for USRP
        self._fusb_nblocks       = options.fusb_nblocks    # usb info for USRP

        if self._tx_freq is None:
            sys.stderr.write("-f FREQ or --freq FREQ or --tx-freq FREQ must be specified\n")
            raise SystemExit

        if self._rx_freq is None:
            sys.stderr.write("-f FREQ or --freq FREQ or --rx-freq FREQ must be specified\n")
            raise SystemExit

        # Set up USRP sink and source
        self._setup_usrp_sink()
        self._setup_usrp_source()

        # Set center frequency of USRP
        ok = self.set_freq(self._tx_freq)
        if not ok:
            print "Failed to set Tx frequency to %s" % (eng_notation.num_to_str(self._tx_freq),)
            raise ValueError

        # copy the final answers back into options for use by modulator
        #options.bitrate = self._bitrate

        self.txpath = transmit_path(self, options)
        self.rxpath = receive_path(self, callback, options)

        self.connect(self.txpath, self.u_snk)
        self.connect(self.u_src, self.rxpath)

    def carrier_sensed(self):
        """
        Return True if the receive path thinks there's carrier
        """
        return self.rxpath.carrier_sensed()

    def _setup_usrp_sink(self):
        """
        Creates a USRP sink, determines the settings for best bitrate,
        and attaches to the transmitter's subdevice.
        """
        self.u_snk = usrp.sink_c(fusb_block_size=self._fusb_block_size,
                                 fusb_nblocks=self._fusb_nblocks)

        self.u_snk.set_interp_rate(self._interp)

        # determine the daughterboard subdevice we're using
        if self._tx_subdev_spec is None:
            self._tx_subdev_spec = usrp.pick_tx_subdevice(self.u_snk)
        self.u_snk.set_mux(usrp.determine_tx_mux_value(self.u_snk, self._tx_subdev_spec))
        self.subdev = usrp.selected_subdev(self.u_snk, self._tx_subdev_spec)

        # Set the USRP for maximum transmit gain
        # (Note that on the RFX cards this is a nop.)
        self.set_gain(self.subdev.gain_range()[1])

        # enable Auto Transmit/Receive switching
        self.set_auto_tr(True)

    def _setup_usrp_source(self):
        self.u_src = usrp.source_c (fusb_block_size=self._fusb_block_size,
                                fusb_nblocks=self._fusb_nblocks)
        adc_rate = self.u_src.adc_rate()

        self.u_src.set_decim_rate(self._decim)

        # determine the daughterboard subdevice we're using
        if self._rx_subdev_spec is None:
            self._rx_subdev_spec = usrp.pick_rx_subdevice(self.u_src)
        self.subdev = usrp.selected_subdev(self.u_src, self._rx_subdev_spec)

        self.u_src.set_mux(usrp.determine_rx_mux_value(self.u_src, self._rx_subdev_spec))

    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital up converter.
        """
        r_snk = self.u_snk.tune(self.subdev._which, self.subdev, target_freq)
        r_src = self.u_src.tune(self.subdev._which, self.subdev, target_freq)
        if r_snk and r_src:
            return True

        return False
        
    def set_gain(self, gain):
        """
        Sets the analog gain in the USRP
        """
        self.gain = gain
        self.subdev.set_gain(gain)

    def set_auto_tr(self, enable):
        """
        Turns on auto transmit/receive of USRP daughterboard (if exits; else ignored)
        """
        return self.subdev.set_auto_tr(enable)
        
    def interp(self):
        return self._interp

    def add_options(normal, expert):
        """
        Adds usrp-specific options to the Options Parser
        """
        add_freq_option(normal)
        normal.add_option("-T", "--tx-subdev-spec", type="subdev", default=None,
                          help="select USRP Tx side A or B")
        normal.add_option("-v", "--verbose", action="store_true", default=False)

        expert.add_option("", "--tx-freq", type="eng_float", default=None,
                          help="set transmit frequency to FREQ [default=%default]", metavar="FREQ")
        expert.add_option("-i", "--interp", type="intx", default=256,
                          help="set fpga interpolation rate to INTERP [default=%default]")
        normal.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B")
        normal.add_option("", "--rx-gain", type="eng_float", default=None, metavar="GAIN",
                          help="set receiver gain in dB [default=midpoint].  See also --show-rx-gain-range")
        normal.add_option("", "--show-rx-gain-range", action="store_true", default=False, 
                          help="print min and max Rx gain available on selected daughterboard")
        normal.add_option("-v", "--verbose", action="store_true", default=False)

        expert.add_option("", "--rx-freq", type="eng_float", default=None,
                          help="set Rx frequency to FREQ [default=%default]", metavar="FREQ")
        expert.add_option("-d", "--decim", type="intx", default=128,
                          help="set fpga decimation rate to DECIM [default=%default]")
        expert.add_option("", "--snr", type="eng_float", default=30,
                          help="set the SNR of the channel in dB [default=%default]")
   
    # Make a static method to call before instantiation
    add_options = staticmethod(add_options)

    def _print_verbage(self):
        """
        Prints information about the transmit path
        """
        print "Using TX d'board %s"    % (self.subdev.side_and_name(),)
        print "modulation:      %s"    % (self._modulator_class.__name__)
        print "interp:          %3d"   % (self._interp)
        print "Tx Frequency:    %s"    % (eng_notation.num_to_str(self._tx_freq))
        
def add_freq_option(parser):
    """
    Hackery that has the -f / --freq option set both tx_freq and rx_freq
    """
    def freq_callback(option, opt_str, value, parser):
        parser.values.rx_freq = value
        parser.values.tx_freq = value

    if not parser.has_option('--freq'):
        parser.add_option('-f', '--freq', type="eng_float",
                          action="callback", callback=freq_callback,
                          help="set Tx and/or Rx frequency to FREQ [default=%default]",
                          metavar="FREQ")


# /////////////////////////////////////////////////////////////////////////////
#                           Carrier Sense MAC
# /////////////////////////////////////////////////////////////////////////////

class cs_mac(object):
    """
    Prototype carrier sense MAC

    Reads packets from the TUN/TAP interface, and sends them to the PHY.
    Receives packets from the PHY via phy_rx_callback, and sends them
    into the TUN/TAP interface.

    Of course, we're not restricted to getting packets via TUN/TAP, this
    is just an example.
    """
    def __init__(self, tun_fd, verbose=False):
        self.tun_fd = tun_fd       # file descriptor for TUN/TAP interface
        self.verbose = verbose
        self.fg = None             # flow graph (access to PHY)

    def set_flow_graph(self, fg):
        self.fg = fg

    def phy_rx_callback(self, ok, payload):
        """
        Invoked by thread associated with PHY to pass received packet up.

        @param ok: bool indicating whether payload CRC was OK
        @param payload: contents of the packet (string)
        """
        if self.verbose:
            print "Rx: ok = %r  len(payload) = %4d" % (ok, len(payload))
        if ok:
            os.write(self.tun_fd, payload)

    def main_loop(self):
        """
        Main loop for MAC.
        Only returns if we get an error reading from TUN.

        FIXME: may want to check for EINTR and EAGAIN and reissue read
        """
        min_delay = 0.001               # seconds

        while 1:
            payload = os.read(self.tun_fd, 10*1024)
            if not payload:
                self.fg.send_pkt(eof=True)
                break

            if self.verbose:
                print "Tx: len(payload) = %4d" % (len(payload),)

            delay = min_delay
            while self.fg.carrier_sensed():
                sys.stderr.write('B')
                time.sleep(delay)
                if delay < 0.050:
                    delay = delay * 2       # exponential back-off

            self.fg.send_pkt(payload)


# /////////////////////////////////////////////////////////////////////////////
#                                   main
# /////////////////////////////////////////////////////////////////////////////

def main():

    parser = OptionParser (option_class=eng_option, conflict_handler="resolve")
    expert_grp = parser.add_option_group("Expert")

    parser.add_option("-m", "--modulation", type="choice", choices=['bpsk', 'qpsk'],
                      default='bpsk',
                      help="Select modulation from: bpsk, qpsk [default=%%default]")
    
    parser.add_option("-v","--verbose", action="store_true", default=False)
    expert_grp.add_option("-c", "--carrier-threshold", type="eng_float", default=30,
                          help="set carrier detect threshold (dB) [default=%default]")
    expert_grp.add_option("","--tun-device-filename", default="/dev/net/tun",
                          help="path to tun device file [default=%default]")

    usrp_graph.add_options(parser, expert_grp)
    transmit_path.add_options(parser, expert_grp)
    receive_path.add_options(parser, expert_grp)
    blks.ofdm_mod.add_options(parser, expert_grp)
    blks.ofdm_demod.add_options(parser, expert_grp)

    fusb_options.add_options(expert_grp)

    (options, args) = parser.parse_args ()
    if len(args) != 0:
        parser.print_help(sys.stderr)
        sys.exit(1)

    if options.rx_freq is None or options.tx_freq is None:
        sys.stderr.write("You must specify -f FREQ or --freq FREQ\n")
        parser.print_help(sys.stderr)
        sys.exit(1)

    # open the TUN/TAP interface
    (tun_fd, tun_ifname) = open_tun_interface(options.tun_device_filename)

    # Attempt to enable realtime scheduling
    r = gr.enable_realtime_scheduling()
    if r == gr.RT_OK:
        realtime = True
    else:
        realtime = False
        print "Note: failed to enable realtime scheduling"


    # If the user hasn't set the fusb_* parameters on the command line,
    # pick some values that will reduce latency.

    if options.fusb_block_size == 0 and options.fusb_nblocks == 0:
        if realtime:                        # be more aggressive
            options.fusb_block_size = gr.prefs().get_long('fusb', 'rt_block_size', 1024)
            options.fusb_nblocks    = gr.prefs().get_long('fusb', 'rt_nblocks', 16)
        else:
            options.fusb_block_size = gr.prefs().get_long('fusb', 'block_size', 4096)
            options.fusb_nblocks    = gr.prefs().get_long('fusb', 'nblocks', 16)
    
    #print "fusb_block_size =", options.fusb_block_size
    #print "fusb_nblocks    =", options.fusb_nblocks

    # instantiate the MAC
    mac = cs_mac(tun_fd, verbose=True)


    # build the graph (PHY)
    fg = usrp_graph(mac.phy_rx_callback, options)

    mac.set_flow_graph(fg)    # give the MAC a handle for the PHY

    #if fg.txpath.bitrate() != fg.rxpath.bitrate():
    #    print "WARNING: Transmit bitrate = %sb/sec, Receive bitrate = %sb/sec" % (
    #        eng_notation.num_to_str(fg.txpath.bitrate()),
    #        eng_notation.num_to_str(fg.rxpath.bitrate()))
             
    print "modulation:     %s"   % (options.modulation,)
    print "freq:           %s"      % (eng_notation.num_to_str(options.tx_freq))
    #print "bitrate:        %sb/sec" % (eng_notation.num_to_str(fg.txpath.bitrate()),)
    #print "samples/symbol: %3d" % (fg.txpath.samples_per_symbol(),)
    #print "interp:         %3d" % (fg.txpath.interp(),)
    #print "decim:          %3d" % (fg.rxpath.decim(),)

    fg.rxpath.set_carrier_threshold(options.carrier_threshold)
    print "Carrier sense threshold:", options.carrier_threshold, "dB"
    
    print
    print "Allocated virtual ethernet interface: %s" % (tun_ifname,)
    print "You must now use ifconfig to set its IP address. E.g.,"
    print
    print "  $ sudo ifconfig %s 192.168.200.1" % (tun_ifname,)
    print
    print "Be sure to use a different address in the same subnet for each machine."
    print


    fg.start()    # Start executing the flow graph (runs in separate threads)

    mac.main_loop()    # don't expect this to return...

    fg.stop()     # but if it does, tell flow graph to stop.
    fg.wait()     # wait for it to finish
                

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass

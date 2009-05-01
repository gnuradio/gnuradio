#
# Copyright 2009 Free Software Foundation, Inc.
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

import generic_usrp

def _add_options(parser, expert):
    """
    Add options to manually choose between usrp or usrp2.
    Add options for usb. Add options common to source and sink.
    @param parser: instance of OptionParser
    """
    #pick usrp or usrp2
    parser.add_option("-u", "--usrpx", type="string", default=None,
                      help="specify which usrp model: 1 for USRP, 2 for USRP2 [default=auto]")
    #fast usb options
    expert.add_option("-B", "--fusb-block-size", type="int", default=0,
                      help="specify fast usb block size [default=%default]")
    expert.add_option("-N", "--fusb-nblocks", type="int", default=0,
                      help="specify number of fast usb blocks [default=%default]")
    #usrp options
    parser.add_option("-w", "--which", type="int", default=0,
                      help="select USRP board [default=%default]")
    #usrp2 options
    parser.add_option("-e", "--interface", type="string", default="eth0",
                      help="Use USRP2 at specified Ethernet interface [default=%default]")
    parser.add_option("-m", "--mac-addr", type="string", default="",
                      help="Use USRP2 at specified MAC address [default=None]")

def add_rx_options(parser, expert=None):
    """
    Add receive specific usrp options.
    @param parser: instance of OptionParser
    """
    parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                      help="select USRP Rx side A or B")
    parser.add_option("--rx-gain", type="eng_float", default=None, metavar="GAIN",
                      help="set receiver gain in dB [default=midpoint].  See also --show-rx-gain-range")
    parser.add_option("--show-rx-gain-range", action="store_true", default=False, 
                      help="print min and max Rx gain available on selected daughterboard")
    parser.add_option("-d", "--decim", type="intx", default=None,
                      help="set fpga decimation rate to DECIM [default=%default]")
    _add_options(parser, expert)

def create_usrp_source(options):
    u = generic_usrp.generic_usrp_source_c(
        usrpx=options.usrpx,
        which=options.which,
        subdev_spec=options.rx_subdev_spec,
        interface=options.interface,
        mac_addr=options.mac_addr,
        fusb_block_size=options.fusb_block_size,
        fusb_nblocks=options.fusb_nblocks,
    )
    if options.show_rx_gain_range:
        print "Rx Gain Range: minimum = %g, maximum = %g, step size = %g"%tuple(u.gain_range())
    return u

def add_tx_options(parser, expert=None):
    """
    Add transmit specific usrp options.
    @param parser: instance of OptionParser
    """
    parser.add_option("-T", "--tx-subdev-spec", type="subdev", default=None,
                      help="select USRP Rx side A or B")
    parser.add_option("--tx-gain", type="eng_float", default=None, metavar="GAIN",
                      help="set transmitter gain in dB [default=midpoint].  See also --show-tx-gain-range")
    parser.add_option("--show-tx-gain-range", action="store_true", default=False, 
                      help="print min and max Tx gain available on selected daughterboard")
    parser.add_option("-i", "--interp", type="intx", default=None,
                      help="set fpga interpolation rate to INTERP [default=%default]")
    _add_options(parser, expert)

def create_usrp_sink(options):
    u = generic_usrp.generic_usrp_sink_c(
        usrpx=options.usrpx,
        which=options.which,
        subdev_spec=options.tx_subdev_spec,
        interface=options.interface,
        mac_addr=options.mac_addr,
        fusb_block_size=options.fusb_block_size,
        fusb_nblocks=options.fusb_nblocks,
    )
    if options.show_tx_gain_range:
        print "Tx Gain Range: minimum = %g, maximum = %g, step size = %g"%tuple(u.gain_range())
    return u

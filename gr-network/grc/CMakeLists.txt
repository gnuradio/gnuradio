# Copyright 2020 Free Software Foundation, Inc.
#
# This file was generated by gr_modtool, a tool from the GNU Radio framework
# This file is a part of gr-network
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

install(
    FILES network_socket_pdu.block.yml network_tcp_sink.block.yml
          network_tcp_source.block.yml network_tuntap_pdu.block.yml
          network_udp_sink.block.yml network_udp_source.block.yml
    DESTINATION share/gnuradio/grc/blocks)

# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

install(
    FILES test_tcp_sink_client.grc
          test_tcp_sink_client_ipv6.grc
          test_tcp_sink_server.grc
          test_tcp_source.grc
          test_tcp_source_ipv6.grc
          test_udp_sink.grc
          test_udp_sink_ipv6.grc
          test_udp_sink_seq.grc
          test_udp_source.grc
          test_udp_source_ipv6.grc
          test_udp_source_seq.grc
    DESTINATION ${GR_PKG_NETWORK_EXAMPLES_DIR})

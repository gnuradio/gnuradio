#Copyright 2020 Free Software Foundation, Inc.
#
#This file is a part of gnuradio
#
#SPDX - License - Identifier : GPL - 3.0 - or -later
#

########################################################################
#Install public header files
########################################################################
install(FILES
    api.h
    packet_headers.h
    socket_pdu.h
    tcp_sink.h
    tuntap_pdu.h
    udp_header_types.h
    udp_sink.h
    udp_source.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/network
)

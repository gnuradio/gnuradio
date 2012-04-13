# Copyright 2010-2011 Free Software Foundation, Inc.
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

########################################################################
# Generate inet.h header file
########################################################################
include(TestBigEndian)
enable_language(C)
TEST_BIG_ENDIAN(GR_ARCH_BIGENDIAN)

include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(byteswap.h GR_HAVE_BYTESWAP)
CHECK_INCLUDE_FILE_CXX(arpa/inet.h GR_HAVE_ARPA_INET)
CHECK_INCLUDE_FILE_CXX(netinet/in.h GR_HAVE_NETINET_IN)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/inet.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/inet.h
@ONLY)

########################################################################
# Install the headers
########################################################################
install(FILES
    api.h
    attributes.h
    high_res_timer.h
    ${CMAKE_CURRENT_BINARY_DIR}/inet.h
    msg_accepter.h
    msg_accepter_msgq.h
    msg_queue.h
    msg_passing.h
    pmt.h
    pmt_pool.h
    pmt_sugar.h
    realtime.h
    sys_pri.h
    thread_body_wrapper.h
    thread_group.h
    thread.h
DESTINATION ${GR_INCLUDE_DIR}/gruel COMPONENT "gruel_devel")

# Copyright 2012 Free Software Foundation, Inc.
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
INCLUDE(GrMiscUtils)
INCLUDE(CheckCXXSourceCompiles)

IF(MSVC) #add this directory for our provided headers
LIST(APPEND CMAKE_REQUIRED_INCLUDES ${CMAKE_SOURCE_DIR}/msvc)
ENDIF(MSVC)

GR_CHECK_HDR_N_DEF(netdb.h HAVE_NETDB_H)
GR_CHECK_HDR_N_DEF(sys/time.h HAVE_SYS_TIME_H)
GR_CHECK_HDR_N_DEF(sys/types.h HAVE_SYS_TYPES_H)
GR_CHECK_HDR_N_DEF(sys/select.h HAVE_SYS_SELECT_H)
GR_CHECK_HDR_N_DEF(sys/socket.h HAVE_SYS_SOCKET_H)
GR_CHECK_HDR_N_DEF(io.h HAVE_IO_H)
GR_CHECK_HDR_N_DEF(sys/mman.h HAVE_SYS_MMAN_H)
GR_CHECK_HDR_N_DEF(sys/ipc.h HAVE_SYS_IPC_H)
GR_CHECK_HDR_N_DEF(sys/shm.h HAVE_SYS_SHM_H)
GR_CHECK_HDR_N_DEF(signal.h HAVE_SIGNAL_H)
GR_CHECK_HDR_N_DEF(netinet/in.h HAVE_NETINET_IN_H)
GR_CHECK_HDR_N_DEF(arpa/inet.h HAVE_ARPA_INET_H)
GR_CHECK_HDR_N_DEF(byteswap.h HAVE_BYTESWAP_H)
GR_CHECK_HDR_N_DEF(linux/ppdev.h HAVE_LINUX_PPDEV_H)
GR_CHECK_HDR_N_DEF(dev/ppbus/ppi.h HAVE_DEV_PPBUS_PPI_H)
GR_CHECK_HDR_N_DEF(unistd.h HAVE_UNISTD_H)
GR_CHECK_HDR_N_DEF(malloc.h HAVE_MALLOC_H)

########################################################################
CHECK_CXX_SOURCE_COMPILES("
    #include <sys/select.h>
    int main(){select(0, 0, 0, 0, 0); return 0;}
    " HAVE_SELECT
)
GR_ADD_COND_DEF(HAVE_SELECT)

########################################################################
CHECK_INCLUDE_FILE_CXX(windows.h HAVE_WINDOWS_H)
IF(HAVE_WINDOWS_H)
    ADD_DEFINITIONS(-DHAVE_WINDOWS_H -DUSING_WINSOCK)
    MESSAGE(STATUS "Adding windows libs to gr blocks libs...")
    LIST(APPEND blocks_libs ws2_32 wsock32)
ENDIF(HAVE_WINDOWS_H)

########################################################################
CHECK_CXX_SOURCE_COMPILES("
    #define _GNU_SOURCE
    #include <math.h>
    int main(){double x, sin, cos; sincos(x, &sin, &cos); return 0;}
    " HAVE_SINCOS
)
GR_ADD_COND_DEF(HAVE_SINCOS)

CHECK_CXX_SOURCE_COMPILES("
    #define _GNU_SOURCE
    #include <math.h>
    int main(){float x, sin, cos; sincosf(x, &sin, &cos); return 0;}
    " HAVE_SINCOSF
)
GR_ADD_COND_DEF(HAVE_SINCOSF)

CHECK_CXX_SOURCE_COMPILES("
    #include <math.h>
    int main(){sinf(0); return 0;}
    " HAVE_SINF
)
GR_ADD_COND_DEF(HAVE_SINF)

CHECK_CXX_SOURCE_COMPILES("
    #include <math.h>
    int main(){cosf(0); return 0;}
    " HAVE_COSF
)
GR_ADD_COND_DEF(HAVE_COSF)

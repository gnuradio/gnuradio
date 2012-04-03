# Copyright 2011 Free Software Foundation, Inc.
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

# DEMOD
install(
    FILES
    demod/digital_freq_lock.grc
    demod/mpsk_demod.grc
    demod/pam_sync.grc
    demod/pam_timing.grc
    DESTINATION ${GR_PKG_DATA_DIR}/examples/demod
    COMPONENT "gnuadio-examples"
)

# SIMPLE
install(
    FILES
    simple/ber_simulation.grc
    simple/dpsk_loopback.grc
    simple/variable_config.grc
    DESTINATION ${GR_PKG_DATA_DIR}/examples/simple
    COMPONENT "gnuadio-examples"
)

# XMLRPC
install(
    FILES
    xmlrpc/readme.txt
    xmlrpc/xmlrpc_client.grc
    xmlrpc/xmlrpc_client_script.py
    xmlrpc/xmlrpc_server.grc
    DESTINATION ${GR_PKG_DATA_DIR}/examples/xmlrpc
    COMPONENT "gnuadio-examples"
)

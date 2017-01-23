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
include(GrPython)

install(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/icon.png
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport
    COMPONENT "runtime_python"
)


GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
    ${CMAKE_CURRENT_SOURCE_DIR}/GrDataPlotter.py
    ${CMAKE_CURRENT_SOURCE_DIR}/monitor.py
    ${CMAKE_CURRENT_SOURCE_DIR}/GNURadioControlPortClient.py
    ${CMAKE_CURRENT_SOURCE_DIR}/RPCConnection.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport/
    COMPONENT "runtime_python"
)

GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gr-perf-monitorx
    ${CMAKE_CURRENT_SOURCE_DIR}/gr-ctrlport-monitor
    DESTINATION ${GR_RUNTIME_DIR}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    COMPONENT "runtime_python"
)

if(THRIFT_FOUND)

  list(APPEND thrift_targets
    ${CMAKE_CURRENT_BINARY_DIR}/GNURadio/__init__.py
    ${CMAKE_CURRENT_BINARY_DIR}/GNURadio/constants.py
    ${CMAKE_CURRENT_BINARY_DIR}/GNURadio/ControlPort.py
    ${CMAKE_CURRENT_BINARY_DIR}/GNURadio/ControlPort-remote
    ${CMAKE_CURRENT_BINARY_DIR}/GNURadio/StreamReceiver.py
    ${CMAKE_CURRENT_BINARY_DIR}/GNURadio/StreamReceiver-remote
    ${CMAKE_CURRENT_BINARY_DIR}/GNURadio/ttypes.py
    )
  add_custom_command(
    DEPENDS ${CMAKE_SOURCE_DIR}/gnuradio-runtime/lib/controlport/thrift/gnuradio.thrift
    OUTPUT ${thrift_targets}
    COMMAND ${THRIFT_BIN} --gen py -out ${CMAKE_CURRENT_BINARY_DIR}/ ${CMAKE_SOURCE_DIR}/gnuradio-runtime/lib/controlport/thrift/gnuradio.thrift
    )

  GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/RPCConnectionThrift.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport/
    COMPONENT "runtime_python"
    )

  GR_PYTHON_INSTALL(
    FILES
    ${thrift_targets}
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport/GNURadio
    COMPONENT "runtime_python"
    )

endif(THRIFT_FOUND)

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

EXECUTE_PROCESS(
  COMMAND ${ICE_SLICE2PY} -I${CMAKE_SOURCE_DIR}/gnuradio-runtime/lib/controlport
          --output-dir=${CMAKE_BINARY_DIR}/gnuradio-runtime/python
          ${CMAKE_SOURCE_DIR}/gnuradio-runtime/lib/controlport/gnuradio.ice
)

EXECUTE_PROCESS(
  COMMAND ${ICE_SLICE2PY} -I${CMAKE_SOURCE_DIR}/gnuradio-runtime/lib/controlport
          --output-dir=${CMAKE_BINARY_DIR}/gnuradio-runtime/python
          ${CMAKE_SOURCE_DIR}/gnuradio-runtime/lib/controlport/frontend.ice
)

EXECUTE_PROCESS(
  COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/gnuradio-runtime/python/gnuradio/__init__.py
)

GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
    ${CMAKE_CURRENT_SOURCE_DIR}/IceRadioClient.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport/
    COMPONENT "runtime_python"
)

# We don't want to install these in the root Python directory, but we
# aren't given a choice based on the way slice2py generates the
# information.
GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_BINARY_DIR}/gnuradio-runtime/python/gnuradio_ice.py
    ${CMAKE_BINARY_DIR}/gnuradio-runtime/python/frontend_ice.py
    DESTINATION ${GR_PYTHON_DIR}
    COMPONENT "runtime_python"
)

GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/GNURadio/__init__.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport/GNURadio
    COMPONENT "runtime_python"
)

GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/GNURadio/Booter/__init__.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport/GNURadio/Booter
    COMPONENT "runtime_python"
)

GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/GNURadio/Frontend/__init__.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport/GNURadio/Frontend
    COMPONENT "runtime_python"
)

install(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/icon.png
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport
    COMPONENT "runtime_python"
)

GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/GrDataPlotter.py
    ${CMAKE_CURRENT_SOURCE_DIR}/monitor.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport/
    COMPONENT "runtime_python"
)

GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gr-ctrlport-monitor
    ${CMAKE_CURRENT_SOURCE_DIR}/gr-perf-monitorx
    ${CMAKE_CURRENT_SOURCE_DIR}/gr-ctrlport-curses
    DESTINATION ${GR_RUNTIME_DIR}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
    COMPONENT "runtime_python"
)

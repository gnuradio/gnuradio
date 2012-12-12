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
  COMMAND ${ICE_SLICE2PY} -I${CMAKE_SOURCE_DIR}/gnuradio-core/src/lib/runtime
          --output-dir=${CMAKE_BINARY_DIR}/gnuradio-core/src/python
          ${CMAKE_SOURCE_DIR}/gnuradio-core/src/lib/runtime/gnuradio.ice
)

EXECUTE_PROCESS(
  COMMAND ${ICE_SLICE2PY} -I${CMAKE_SOURCE_DIR}/gnuradio-core/src/lib/runtime
          --output-dir=${CMAKE_BINARY_DIR}/gnuradio-core/src/python
          ${CMAKE_SOURCE_DIR}/gnuradio-core/src/lib/runtime/frontend.ice
)

GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
    ${CMAKE_CURRENT_SOURCE_DIR}/IceRadioClient.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport/
    COMPONENT "core_python"
)

# We don't want to install these in the root Python directory, but we
# aren't given a choice based on the way slice2py generates the
# information.
GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_BINARY_DIR}/gnuradio-core/src/python/gnuradio_ice.py
    ${CMAKE_BINARY_DIR}/gnuradio-core/src/python/frontend_ice.py
    DESTINATION ${GR_PYTHON_DIR}
    COMPONENT "core_python"
)

GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/GNURadio/__init__.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport/GNURadio
    COMPONENT "core_python"
)

GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/GNURadio/Booter/__init__.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport/GNURadio/Booter
    COMPONENT "core_python"
)

GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/GNURadio/Frontend/__init__.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport/GNURadio/Frontend
    COMPONENT "core_python"
)

install(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/icon.png
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport
    COMPONENT "core_python"
)

GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/DataPlotter.py
    ${CMAKE_CURRENT_SOURCE_DIR}/GrDataPlotter.py
    ${CMAKE_CURRENT_SOURCE_DIR}/monitor.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ctrlport/
    COMPONENT "core_python"
)

GR_PYTHON_INSTALL(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/ctrlport-monitor
    ${CMAKE_CURRENT_SOURCE_DIR}/gr-ctrlport-monitor
    ${CMAKE_CURRENT_SOURCE_DIR}/gr-ctrlport-curses
    DESTINATION ${GR_RUNTIME_DIR}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
    COMPONENT "core_python"
)

########################################################################
# Handle the unit tests
########################################################################
if(ENABLE_GR_CTRLPORT)
  if(ENABLE_TESTING)
    include(GrTest)
    file(GLOB py_qa_test_files "qa_*.py")
    foreach(py_qa_test_file ${py_qa_test_files})
      get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
      set(GR_TEST_TARGET_DEPS gruel gnuradio-core)
      GR_ADD_TEST(${py_qa_test_name} ${PYTHON_EXECUTABLE} ${py_qa_test_file})
    endforeach(py_qa_test_file)
  endif(ENABLE_TESTING)
endif(ENABLE_GR_CTRLPORT)

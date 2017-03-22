# Copyright 2010-2013 Free Software Foundation, Inc.
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
include(GrMiscUtils) #check n def
GR_CHECK_HDR_N_DEF(sys/resource.h HAVE_SYS_RESOURCE_H)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIRS}
)

link_directories(
    ${Boost_LIBRARY_DIRS}
    ${CPPUNIT_LIBRARY_DIRS}
)

########################################################################
# Build benchmarks and non-registered tests
########################################################################
set(tests_not_run #single source per test
    benchmark_nco.cc
    benchmark_vco.cc
)

foreach(test_not_run_src ${tests_not_run})
    get_filename_component(name ${test_not_run_src} NAME_WE)
    add_executable(${name} ${test_not_run_src})
    target_link_libraries(${name} test-gnuradio-runtime gnuradio-blocks)
endforeach(test_not_run_src)

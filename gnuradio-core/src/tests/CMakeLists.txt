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
INCLUDE(GrMiscUtils) #check n def
GR_CHECK_HDR_N_DEF(sys/resource.h HAVE_SYS_RESOURCE_H)

########################################################################
# Setup the include and linker paths
########################################################################
INCLUDE_DIRECTORIES(${GRUEL_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GNURADIO_CORE_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIRS})
LINK_DIRECTORIES(${CPPUNIT_LIBRARY_DIRS})

########################################################################
# Build benchmarks and non-registered tests
########################################################################
SET(tests_not_run #single source per test
    benchmark_dotprod_fff.cc
    benchmark_dotprod_fsf.cc
    benchmark_dotprod_ccf.cc
    benchmark_dotprod_fcc.cc
    benchmark_dotprod_scc.cc
    benchmark_dotprod_ccc.cc
    benchmark_nco.cc
    benchmark_vco.cc
    test_runtime.cc
    test_general.cc
    test_filter.cc
    #test_atsc.cc
    test_vmcircbuf.cc
)

FOREACH(test_not_run_src ${tests_not_run})
    GET_FILENAME_COMPONENT(name ${test_not_run_src} NAME_WE)
    ADD_EXECUTABLE(${name} ${test_not_run_src})
    TARGET_LINK_LIBRARIES(${name} test-gnuradio-core)
ENDFOREACH(test_not_run_src)

########################################################################
# Build the test-all test to end all tests
# Set the test environment so the build libs will be found under MSVC.
########################################################################
INCLUDE(GrTest)
SET(GR_TEST_TARGET_DEPS gruel gnuradio-core test-gnuradio-core)
ADD_EXECUTABLE(gr_core_test_all test_all.cc)
TARGET_LINK_LIBRARIES(gr_core_test_all test-gnuradio-core)
GR_ADD_TEST(gr-core-test-all gr_core_test_all)

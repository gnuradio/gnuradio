# Copyright 2010-2013,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
include(GrMiscUtils) #check n def
gr_check_hdr_n_def(sys/resource.h HAVE_SYS_RESOURCE_H)

########################################################################
# Build benchmarks and non-registered tests
########################################################################
set(tests_not_run #single source per test
    benchmark_nco.cc
    benchmark_vco.cc
    )

foreach(test_not_run_src ${tests_not_run})
    get_filename_component(test_name ${test_not_run_src} NAME_WE)
    add_executable(${test_name} ${test_not_run_src})
    message(STATUS "Blocks/tests: adding ${test_name}")
    target_link_libraries(${test_name} PRIVATE gnuradio-runtime gnuradio-blocks)
endforeach(test_not_run_src)

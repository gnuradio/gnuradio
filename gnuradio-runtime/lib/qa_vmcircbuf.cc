/* -*- c++ -*- */
/*
 * Copyright 2002,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "vmcircbuf.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_all)
{
    int verbose = 1; // summary

    BOOST_REQUIRE(gr::vmcircbuf_sysconfig::test_all_factories(verbose));
}

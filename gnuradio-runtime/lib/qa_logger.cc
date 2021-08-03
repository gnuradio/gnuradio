/*
 * Copyright 2012, 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * This class gathers together all the test cases for the example
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/logger.h>
#include <boost/test/unit_test.hpp>
#include <memory>

BOOST_AUTO_TEST_CASE(t1)
{
    // This doesn't really test anything, more just
    // making sure nothing's gone horribly wrong.
    auto log = std::make_shared<gr::logger>("main");
    GR_LOG_NOTICE(log, "test from c++ NOTICE");
    GR_LOG_DEBUG(log, "test from c++ DEBUG");
    GR_LOG_INFO(log, "test from c++ INFO");
    GR_LOG_WARN(log, "test from c++ WARN");
    GR_LOG_ERROR(log, "test from c++ ERROR");
    GR_LOG_FATAL(log, "test from c++ FATAL");
    BOOST_CHECK(true);
}

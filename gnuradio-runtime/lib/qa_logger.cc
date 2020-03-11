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

BOOST_AUTO_TEST_CASE(t1)
{
#ifdef ENABLE_GR_LOG
    // This doesn't really test anything, more just
    // making sure nothing's gone horribly wrong.

    GR_LOG_GETLOGGER(LOG, "main");
    GR_ADD_CONSOLE_APPENDER("main", "cout", "%d{%H:%M:%S} : %m%n");
    GR_LOG_NOTICE(LOG, "test from c++ NOTICE");
    GR_LOG_DEBUG(LOG, "test from c++ DEBUG");
    GR_LOG_INFO(LOG, "test from c++ INFO");
    GR_LOG_WARN(LOG, "test from c++ WARN");
    GR_LOG_ERROR(LOG, "test from c++ ERROR");
    GR_LOG_FATAL(LOG, "test from c++ FATAL");
    BOOST_CHECK(true);
#endif
}

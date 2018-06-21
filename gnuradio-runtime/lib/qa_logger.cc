/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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

BOOST_AUTO_TEST_CASE(t1) {
#ifdef ENABLE_GR_LOG
  // This doesn't really test anything, more just
  // making sure nothing's gone horribly wrong.

  GR_LOG_GETLOGGER(LOG,"main");
  GR_ADD_CONSOLE_APPENDER("main","cout","%d{%H:%M:%S} : %m%n");
  GR_LOG_NOTICE(LOG,"test from c++ NOTICE");
  GR_LOG_DEBUG(LOG,"test from c++ DEBUG");
  GR_LOG_INFO(LOG,"test from c++ INFO");
  GR_LOG_WARN(LOG,"test from c++ WARN");
  GR_LOG_ERROR(LOG,"test from c++ ERROR");
  GR_LOG_FATAL(LOG,"test from c++ FATAL");
  BOOST_CHECK(true);
#endif
}

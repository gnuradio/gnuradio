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

#include <qa_gr_log.h>
#include <gr_log.h>

void
qa_gr_log::t1()
{
#ifdef ENABLE_GR_LOG
  // This doesn't really test anything, more just
  // making sure nothing's gone horribly wrong.
  GR_LOG_GETLOGGER(LOG,"errLoggerRoot");
  GR_LOG_TRACE(LOG,"test from c++ 1");
  GR_LOG_DEBUG(LOG,"test from c++ 1");
  GR_LOG_INFO(LOG,"test from c++ 1");
  GR_LOG_WARN(LOG,"test from c++ 1");
  GR_LOG_ERROR(LOG,"test from c++ 1");
  GR_LOG_FATAL(LOG,"test from c++ 1");
  GR_LOG_ERRORIF(LOG,2>1,"test from c++ 1");
  CPPUNIT_ASSERT(true);
#endif
}

/* -*- c++ -*- */
/*
 * Copyright 2016 Andrew Jeffery
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
#ifndef _QA_PAGER_FLEX_PARSE_IMPL_H_
#define _QA_PAGER_FLEX_PARSE_IMPL_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

namespace gr {
  namespace pager {

    class qa_flex_parse_impl : public CppUnit::TestCase {

      CPPUNIT_TEST_SUITE(qa_flex_parse_impl);
      CPPUNIT_TEST(inactive_frame);
      CPPUNIT_TEST(inactive_page);
      CPPUNIT_TEST(simple_aln);
      CPPUNIT_TEST(simple_standard_num);
      CPPUNIT_TEST(simple_numbered_num);
      CPPUNIT_TEST(simple_laddr_aln);
      CPPUNIT_TEST(simple_laddr_num);
      CPPUNIT_TEST(simple_laddr_saddr);
      CPPUNIT_TEST_SUITE_END();

    private:
      void inactive_frame();
      void inactive_page();
      void simple_aln();
      void simple_standard_num();
      void simple_numbered_num();
      void simple_laddr_aln();
      void simple_laddr_num();
      void simple_laddr_saddr();
    };

  } /* namespace pager */
} /* namespace gr */

#endif /* _QA_FLEX_PARSE_IMPL_H_ */

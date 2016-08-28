/* -*- c++ -*- */
/*
 * Copyright 2016, Andrew Jeffery
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cppunit/TestCase.h>

#include "qa_flex_parse_impl.h"
#include "flex_parse_impl.h"
#include <gnuradio/msg_queue.h>
#include <gnuradio/blocks/message_source.h>
#include <gnuradio/blocks/message_sink.h>

uint32_t inactive_frame_data[88] = {
  0x0000040b, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

uint32_t inactive_page_data[88] = {
  0x0000080b, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

uint32_t simple_aln_data[88] = {
  0x00000807, 0x001d7edd, 0x0004c1d1, 0x00001a04, 0x00122a06, 0x000829c9,
  0x000829c9, 0x00151041, 0x001529c5, 0x00116820, 0x0013e4d2, 0x0010e4c4,
  0x00106820, 0x000822c7, 0x001462d3, 0x0013a2d5, 0x001064d4, 0x0013904c,
  0x0010a6d5, 0x00082945, 0x000d5c20, 0x0000da33, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

std::string simple_aln_expected("148.812""\x80""   1900253""\x80""ALN""\x80""THIS IS A TEST PERIODIC PAGE SEQUENTIAL NUMBER  8534");

uint32_t simple_standard_num_data[88] = {
  0x0000080b, 0x001df8a1, 0x000001b6, 0x0014C840, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

std::string simple_standard_num_expected("148.812""\x80""   1931425""\x80""NUM""\x80""0123");

uint32_t simple_numbered_num_data[88] = {
  0x0000080b, 0x001df8a1, 0x000001f6, 0x0014C840, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

std::string simple_numbered_num_expected("148.812""\x80""   1931425""\x80""NNM""\x80""23");

uint32_t simple_laddr_aln_data[88] = {
  0x00000C0B, 0x00008000, 0x001F8000, 0x000082D0, 0x00000800, 0x0010E141,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

std::string simple_laddr_aln_expected("148.812""\x80""1075810304""\x80""ALN""\x80""ABC");

uint32_t simple_laddr_num_data[88] = {
  0x00000C0B, 0x00008000, 0x001F8000, 0x00000236, 0x0014C840, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

std::string simple_laddr_num_expected("148.812""\x80""1075810304""\x80""NUM""\x80""0123");

uint32_t simple_laddr_saddr_data[88] = {
  0x0000100B, 0x00008000, 0x001F8000, 0x001d7edd, 0x00008450, 0x00000000,
  0x0004c4d1, 0x00000800, 0x0010E141, 0x00001a04, 0x00122a06, 0x000829c9,
  0x000829c9, 0x00151041, 0x001529c5, 0x00116820, 0x0013e4d2, 0x0010e4c4,
  0x00106820, 0x000822c7, 0x001462d3, 0x0013a2d5, 0x001064d4, 0x0013904c,
  0x0010a6d5, 0x00082945, 0x000d5c20, 0x0000da33, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

std::string simple_laddr_saddr_expected[] = {
  std::string("148.812""\x80""1075810304""\x80""ALN""\x80""ABC"),
  std::string("148.812""\x80""   1900253""\x80""ALN""\x80""THIS IS A TEST PERIODIC PAGE SEQUENTIAL NUMBER  8534"),
};

#define ASSERT_QHEAD_STREQ(e, q) \
  do { \
    std::string __e_evaluated = e; \
    std::string __found = (q)->delete_head()->to_string(); \
    int __cmp = (__e_evaluated).compare(__found); \
    CPPUNIT_ASSERT(__cmp == 0); \
  } while(0);

namespace gr {
  namespace pager {

    static const unsigned int frequency = 148812000;

    void qa_flex_parse_impl::inactive_frame() {
      int i;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&inactive_frame_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(0 == q->count());
    }

    void qa_flex_parse_impl::inactive_page() {
      int i;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&inactive_page_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(0 == q->count());
    }

    void qa_flex_parse_impl::simple_aln() {
      int i;
      std::string parsed;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&simple_aln_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(1 == q->count());

      ASSERT_QHEAD_STREQ(simple_aln_expected, q);
    }

    void qa_flex_parse_impl::simple_standard_num() {
      int i;
      std::string parsed;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&simple_standard_num_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(1 == q->count());

      ASSERT_QHEAD_STREQ(simple_standard_num_expected, q);
    }

    void qa_flex_parse_impl::simple_numbered_num() {
      int i;
      std::string parsed;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&simple_numbered_num_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(1 == q->count());

      ASSERT_QHEAD_STREQ(simple_numbered_num_expected, q);
    }

    void qa_flex_parse_impl::simple_laddr_aln() {
      int i;
      std::string parsed;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&simple_laddr_aln_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(1 == q->count());

      ASSERT_QHEAD_STREQ(simple_laddr_aln_expected, q);
    }

    void qa_flex_parse_impl::simple_laddr_num() {
      int i;
      std::string parsed;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&simple_laddr_num_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(1 == q->count());

      ASSERT_QHEAD_STREQ(simple_laddr_num_expected, q);
    }

    void qa_flex_parse_impl::simple_laddr_saddr() {
      int i;
      std::string parsed;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&simple_laddr_saddr_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(2 == q->count());

      for (i = 0; i < 2; i++) {
        ASSERT_QHEAD_STREQ(simple_laddr_saddr_expected[i], q);
      }
    }
  };
};

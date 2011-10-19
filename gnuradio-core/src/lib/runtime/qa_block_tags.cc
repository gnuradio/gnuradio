/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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
#include <qa_block_tags.h>
#include <gr_block.h>
#include <gr_top_block.h>
#include <gr_null_source.h>
#include <gr_null_sink.h>
#include <gr_head.h>
#include <gr_annotator_alltoall.h>
#include <gr_annotator_1to1.h>
#include <gr_keep_one_in_n.h>
#include <gr_firdes.h>
#include <gruel/pmt.h>


// ----------------------------------------------------------------

using namespace pmt;

// set to 1 to turn on debug output
// The debug output fully checks that the tags seen are what are expected. While
// this behavior currently works with our implementation, there is no guarentee
// that the tags will be coming in this specific order, so it's dangerous to
// rely on this as a test of the tag system working. We would really want to
// tags we know we should see and then test that they all occur once, but in no
// particular order.
#define QA_TAGS_DEBUG 0

void
qa_block_tags::t0 ()
{
  unsigned int N = 1000;
  gr_top_block_sptr tb = gr_make_top_block("top");
  gr_block_sptr src (gr_make_null_source(sizeof(int)));
  gr_block_sptr head (gr_make_head(sizeof(int), N));
  gr_block_sptr snk (gr_make_null_sink(sizeof(int)));
  
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, snk, 0);

  //CPPUNIT_ASSERT_THROW(src->nitems_read(0), std::runtime_error);
  //CPPUNIT_ASSERT_THROW(src->nitems_written(0), std::runtime_error);
  CPPUNIT_ASSERT_EQUAL(src->nitems_read(0), (uint64_t)0);
  CPPUNIT_ASSERT_EQUAL(src->nitems_written(0), (uint64_t)0);

  tb->run();

  CPPUNIT_ASSERT_THROW(src->nitems_read(0), std::invalid_argument);
  CPPUNIT_ASSERT(src->nitems_written(0) >= N);
  CPPUNIT_ASSERT_EQUAL(snk->nitems_read(0), (uint64_t)1000);
  CPPUNIT_ASSERT_THROW(snk->nitems_written(0), std::invalid_argument);
}


void
qa_block_tags::t1 ()
{
  int N = 40000;
  gr_top_block_sptr tb = gr_make_top_block("top");
  gr_block_sptr src (gr_make_null_source(sizeof(int)));
  gr_block_sptr head (gr_make_head(sizeof(int), N));
  gr_annotator_alltoall_sptr ann0 (gr_make_annotator_alltoall(10000, sizeof(int)));
  gr_annotator_alltoall_sptr ann1 (gr_make_annotator_alltoall(10000, sizeof(int)));
  gr_annotator_alltoall_sptr ann2 (gr_make_annotator_alltoall(10000, sizeof(int)));
  gr_annotator_alltoall_sptr ann3 (gr_make_annotator_alltoall(10000, sizeof(int)));
  gr_annotator_alltoall_sptr ann4 (gr_make_annotator_alltoall(10000, sizeof(int)));
  gr_block_sptr snk0 (gr_make_null_sink(sizeof(int)));
  gr_block_sptr snk1 (gr_make_null_sink(sizeof(int)));
  
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, ann0, 0);

  tb->connect(ann0, 0, ann1, 0);
  tb->connect(ann0, 1, ann2, 0);
  tb->connect(ann1, 0, ann3, 0);
  tb->connect(ann2, 0, ann4, 0);

  tb->connect(ann3, 0, snk0, 0);
  tb->connect(ann4, 0, snk1, 0);

  tb->run();

  std::vector<gr_tag_t> tags0 = ann0->data();
  std::vector<gr_tag_t> tags3 = ann3->data();
  std::vector<gr_tag_t> tags4 = ann4->data();

  // The first annotator does not receive any tags from the null sink upstream
  CPPUNIT_ASSERT_EQUAL(tags0.size(), (size_t)0);
  CPPUNIT_ASSERT_EQUAL(tags3.size(), (size_t)8);
  CPPUNIT_ASSERT_EQUAL(tags4.size(), (size_t)8);

#if QA_TAGS_DEBUG
  // Kludge together the tags that we know should result from the above graph
  std::stringstream str0, str1, str2;
  str0 << ann0->name() << ann0->unique_id();
  str1 << ann1->name() << ann1->unique_id();
  str2 << ann2->name() << ann2->unique_id();

  pmt_t expected_tags3[8];
  expected_tags3[0] = mp(pmt_from_uint64(0),     mp(str1.str()), mp("seq"), mp(0));
  expected_tags3[1] = mp(pmt_from_uint64(0),     mp(str0.str()), mp("seq"), mp(0));
  expected_tags3[2] = mp(pmt_from_uint64(10000), mp(str1.str()), mp("seq"), mp(1));
  expected_tags3[3] = mp(pmt_from_uint64(10000), mp(str0.str()), mp("seq"), mp(2));
  expected_tags3[4] = mp(pmt_from_uint64(20000), mp(str1.str()), mp("seq"), mp(2));
  expected_tags3[5] = mp(pmt_from_uint64(20000), mp(str0.str()), mp("seq"), mp(4));
  expected_tags3[6] = mp(pmt_from_uint64(30000), mp(str1.str()), mp("seq"), mp(3));
  expected_tags3[7] = mp(pmt_from_uint64(30000), mp(str0.str()), mp("seq"), mp(6));

  pmt_t expected_tags4[8];
  expected_tags4[0] = mp(pmt_from_uint64(0),     mp(str2.str()), mp("seq"), mp(0));
  expected_tags4[1] = mp(pmt_from_uint64(0),     mp(str0.str()), mp("seq"), mp(1));
  expected_tags4[2] = mp(pmt_from_uint64(10000), mp(str2.str()), mp("seq"), mp(1));
  expected_tags4[3] = mp(pmt_from_uint64(10000), mp(str0.str()), mp("seq"), mp(3));
  expected_tags4[4] = mp(pmt_from_uint64(20000), mp(str2.str()), mp("seq"), mp(2));
  expected_tags4[5] = mp(pmt_from_uint64(20000), mp(str0.str()), mp("seq"), mp(5));
  expected_tags4[6] = mp(pmt_from_uint64(30000), mp(str2.str()), mp("seq"), mp(3));
  expected_tags4[7] = mp(pmt_from_uint64(30000), mp(str0.str()), mp("seq"), mp(7));

  std::cout << std::endl << "qa_block_tags::t1" << std::endl;
  
  // For annotator 3, we know it gets tags from ann0 and ann1, test this
  for(size_t i = 0; i < tags3.size(); i++) {
    std::cout << "tags3[" << i << "] = " << tags3[i] << "\t\t" << expected_tags3[i] << std::endl;
    CPPUNIT_ASSERT_EQUAL(pmt_write_string(tags3[i]), pmt_write_string(expected_tags3[i]));
  }

  // For annotator 4, we know it gets tags from ann0 and ann2, test this
  std::cout << std::endl;
  for(size_t i = 0; i < tags4.size(); i++) {
    std::cout << "tags4[" << i << "] = " << tags4[i] << "\t\t" << expected_tags4[i] << std::endl;
    CPPUNIT_ASSERT_EQUAL(pmt_write_string(tags4[i]), pmt_write_string(expected_tags4[i]));
  }
#endif
}

void
qa_block_tags::t2 ()
{
  int N = 40000;
  gr_top_block_sptr tb = gr_make_top_block("top");
  gr_block_sptr src (gr_make_null_source(sizeof(int)));
  gr_block_sptr head (gr_make_head(sizeof(int), N));
  gr_annotator_alltoall_sptr ann0 (gr_make_annotator_alltoall(10000, sizeof(int)));
  gr_annotator_alltoall_sptr ann1 (gr_make_annotator_alltoall(10000, sizeof(int)));
  gr_annotator_alltoall_sptr ann2 (gr_make_annotator_alltoall(10000, sizeof(int)));
  gr_annotator_alltoall_sptr ann3 (gr_make_annotator_alltoall(10000, sizeof(int)));
  gr_annotator_alltoall_sptr ann4 (gr_make_annotator_alltoall(10000, sizeof(int)));
  gr_block_sptr snk0 (gr_make_null_sink(sizeof(int)));
  gr_block_sptr snk1 (gr_make_null_sink(sizeof(int)));
  gr_block_sptr snk2 (gr_make_null_sink(sizeof(int)));
  
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, ann0, 0);

  tb->connect(ann0, 0, ann1, 0);
  tb->connect(ann0, 1, ann1, 1);
  tb->connect(ann1, 0, ann2, 0);
  tb->connect(ann1, 1, ann3, 0);
  tb->connect(ann1, 2, ann4, 0);

  tb->connect(ann2, 0, snk0, 0);
  tb->connect(ann3, 0, snk1, 0);
  tb->connect(ann4, 0, snk2, 0);

  tb->run();

  std::vector<gr_tag_t> tags0 = ann0->data();
  std::vector<gr_tag_t> tags1 = ann1->data();
  std::vector<gr_tag_t> tags2 = ann2->data();
  std::vector<gr_tag_t> tags3 = ann4->data();
  std::vector<gr_tag_t> tags4 = ann4->data();

  // The first annotator does not receive any tags from the null sink upstream
  CPPUNIT_ASSERT_EQUAL(tags0.size(), (size_t)0);
  CPPUNIT_ASSERT_EQUAL(tags1.size(), (size_t)8);

  // Make sure the rest all have 12 tags
  CPPUNIT_ASSERT_EQUAL(tags2.size(), (size_t)12);
  CPPUNIT_ASSERT_EQUAL(tags3.size(), (size_t)12);
  CPPUNIT_ASSERT_EQUAL(tags4.size(), (size_t)12);


#if QA_TAGS_DEBUG
  // Kludge together the tags that we know should result from the above graph
  std::stringstream str0, str1;
  str0 << ann0->name() << ann0->unique_id();
  str1 << ann1->name() << ann1->unique_id();

  pmt_t expected_tags2[12];
  expected_tags2[0] = mp(pmt_from_uint64(0),     mp(str1.str()), mp("seq"), mp(0));
  expected_tags2[1] = mp(pmt_from_uint64(0),     mp(str0.str()), mp("seq"), mp(0));
  expected_tags2[2] = mp(pmt_from_uint64(0),     mp(str0.str()), mp("seq"), mp(1));
  expected_tags2[3] = mp(pmt_from_uint64(10000), mp(str1.str()), mp("seq"), mp(3));
  expected_tags2[4] = mp(pmt_from_uint64(10000), mp(str0.str()), mp("seq"), mp(2));
  expected_tags2[5] = mp(pmt_from_uint64(10000), mp(str0.str()), mp("seq"), mp(3));
  expected_tags2[6] = mp(pmt_from_uint64(20000), mp(str1.str()), mp("seq"), mp(6));
  expected_tags2[7] = mp(pmt_from_uint64(20000), mp(str0.str()), mp("seq"), mp(4));
  expected_tags2[8] = mp(pmt_from_uint64(20000), mp(str0.str()), mp("seq"), mp(5));
  expected_tags2[9] = mp(pmt_from_uint64(30000), mp(str1.str()), mp("seq"), mp(9));
  expected_tags2[10] = mp(pmt_from_uint64(30000), mp(str0.str()), mp("seq"), mp(6));
  expected_tags2[11] = mp(pmt_from_uint64(30000), mp(str0.str()), mp("seq"), mp(7));

  pmt_t expected_tags4[12];
  expected_tags4[0] = mp(pmt_from_uint64(0),     mp(str1.str()), mp("seq"), mp(2));
  expected_tags4[1] = mp(pmt_from_uint64(0),     mp(str0.str()), mp("seq"), mp(0));
  expected_tags4[2] = mp(pmt_from_uint64(0),     mp(str0.str()), mp("seq"), mp(1));
  expected_tags4[3] = mp(pmt_from_uint64(10000), mp(str1.str()), mp("seq"), mp(5));
  expected_tags4[4] = mp(pmt_from_uint64(10000), mp(str0.str()), mp("seq"), mp(2));
  expected_tags4[5] = mp(pmt_from_uint64(10000), mp(str0.str()), mp("seq"), mp(3));
  expected_tags4[6] = mp(pmt_from_uint64(20000), mp(str1.str()), mp("seq"), mp(8));
  expected_tags4[7] = mp(pmt_from_uint64(20000), mp(str0.str()), mp("seq"), mp(4));
  expected_tags4[8] = mp(pmt_from_uint64(20000), mp(str0.str()), mp("seq"), mp(5));
  expected_tags4[9] = mp(pmt_from_uint64(30000), mp(str1.str()), mp("seq"), mp(11));
  expected_tags4[10] = mp(pmt_from_uint64(30000), mp(str0.str()), mp("seq"), mp(6));
  expected_tags4[11] = mp(pmt_from_uint64(30000), mp(str0.str()), mp("seq"), mp(7));

  std::cout << std::endl << "qa_block_tags::t2" << std::endl;

  // For annotator[2-4], we know it gets tags from ann0 and ann1
  // but the tags from the different outputs of ann1 are different for each.
  // Just testing ann2 and ann4; if they are correct it would be
  // inconceivable for ann3 to have it wrong.
  for(size_t i = 0; i < tags2.size(); i++) {
    std::cout << "tags2[" << i << "] = " << tags2[i] << "\t\t" << expected_tags2[i] << std::endl;
    CPPUNIT_ASSERT_EQUAL(pmt_write_string(tags2[i]), pmt_write_string(expected_tags2[i]));
  }

  std::cout << std::endl;
  for(size_t i = 0; i < tags4.size(); i++) {
    std::cout << "tags2[" << i << "] = " << tags4[i] << "\t\t" << expected_tags4[i] << std::endl;
    CPPUNIT_ASSERT_EQUAL(pmt_write_string(tags4[i]), pmt_write_string(expected_tags4[i]));
  }
#endif
}


void
qa_block_tags::t3 ()
{
  int N = 40000;
  gr_top_block_sptr tb = gr_make_top_block("top");
  gr_block_sptr src (gr_make_null_source(sizeof(int)));
  gr_block_sptr head (gr_make_head(sizeof(int), N));
  gr_annotator_1to1_sptr ann0 (gr_make_annotator_1to1(10000, sizeof(int)));
  gr_annotator_alltoall_sptr ann1 (gr_make_annotator_alltoall(10000, sizeof(int)));
  gr_annotator_alltoall_sptr ann2 (gr_make_annotator_alltoall(10000, sizeof(int)));
  gr_annotator_1to1_sptr ann3 (gr_make_annotator_1to1(10000, sizeof(int)));
  gr_annotator_1to1_sptr ann4 (gr_make_annotator_1to1(10000, sizeof(int)));
  gr_block_sptr snk0 (gr_make_null_sink(sizeof(int)));
  gr_block_sptr snk1 (gr_make_null_sink(sizeof(int)));
  
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, ann0, 0);
  tb->connect(head, 0, ann0, 1);

  tb->connect(ann0, 0, ann1, 0);
  tb->connect(ann0, 1, ann2, 0);
  tb->connect(ann1, 0, ann3, 0);
  tb->connect(ann2, 0, ann4, 0);

  tb->connect(ann3, 0, snk0, 0);
  tb->connect(ann4, 0, snk1, 0);

  tb->run();

  
  std::vector<gr_tag_t> tags0 = ann0->data();
  std::vector<gr_tag_t> tags3 = ann3->data();
  std::vector<gr_tag_t> tags4 = ann4->data();

  // The first annotator does not receive any tags from the null sink upstream
  CPPUNIT_ASSERT_EQUAL(tags0.size(), (size_t)0);
  CPPUNIT_ASSERT_EQUAL(tags3.size(), (size_t)8);
  CPPUNIT_ASSERT_EQUAL(tags4.size(), (size_t)8);

#if QA_TAGS_DEBUG
  // Kludge together the tags that we know should result from the above graph
  std::stringstream str0, str1, str2;
  str0 << ann0->name() << ann0->unique_id();
  str1 << ann1->name() << ann1->unique_id();
  str2 << ann2->name() << ann2->unique_id();

  pmt_t expected_tags3[8];
  expected_tags3[0] = mp(pmt_from_uint64(0),     mp(str1.str()), mp("seq"), mp(0));
  expected_tags3[1] = mp(pmt_from_uint64(0),     mp(str0.str()), mp("seq"), mp(0));
  expected_tags3[2] = mp(pmt_from_uint64(10000), mp(str1.str()), mp("seq"), mp(1));
  expected_tags3[3] = mp(pmt_from_uint64(10000), mp(str0.str()), mp("seq"), mp(2));
  expected_tags3[4] = mp(pmt_from_uint64(20000), mp(str1.str()), mp("seq"), mp(2));
  expected_tags3[5] = mp(pmt_from_uint64(20000), mp(str0.str()), mp("seq"), mp(4));
  expected_tags3[6] = mp(pmt_from_uint64(30000), mp(str1.str()), mp("seq"), mp(3));
  expected_tags3[7] = mp(pmt_from_uint64(30000), mp(str0.str()), mp("seq"), mp(6));

  pmt_t expected_tags4[8];
  expected_tags4[0] = mp(pmt_from_uint64(0),     mp(str2.str()), mp("seq"), mp(0));
  expected_tags4[1] = mp(pmt_from_uint64(0),     mp(str0.str()), mp("seq"), mp(1));
  expected_tags4[2] = mp(pmt_from_uint64(10000), mp(str2.str()), mp("seq"), mp(1));
  expected_tags4[3] = mp(pmt_from_uint64(10000), mp(str0.str()), mp("seq"), mp(3));
  expected_tags4[4] = mp(pmt_from_uint64(20000), mp(str2.str()), mp("seq"), mp(2));
  expected_tags4[5] = mp(pmt_from_uint64(20000), mp(str0.str()), mp("seq"), mp(5));
  expected_tags4[6] = mp(pmt_from_uint64(30000), mp(str2.str()), mp("seq"), mp(3));
  expected_tags4[7] = mp(pmt_from_uint64(30000), mp(str0.str()), mp("seq"), mp(7));

  std::cout << std::endl << "qa_block_tags::t3" << std::endl;
  
  // For annotator 3, we know it gets tags from ann0 and ann1, test this
  for(size_t i = 0; i < tags3.size(); i++) {
    std::cout << "tags3[" << i << "] = " << tags3[i] << "\t\t" << expected_tags3[i] << std::endl;
    CPPUNIT_ASSERT_EQUAL(pmt_write_string(tags3[i]), pmt_write_string(expected_tags3[i]));
  }

  // For annotator 4, we know it gets tags from ann0 and ann2, test this
  std::cout << std::endl;
  for(size_t i = 0; i < tags4.size(); i++) {
    std::cout << "tags4[" << i << "] = " << tags4[i] << "\t\t" << expected_tags4[i] << std::endl;
    CPPUNIT_ASSERT_EQUAL(pmt_write_string(tags4[i]), pmt_write_string(expected_tags4[i]));
  }
#endif
}


void
qa_block_tags::t4 ()
{
  int N = 40000;
  gr_top_block_sptr tb = gr_make_top_block("top");
  gr_block_sptr src (gr_make_null_source(sizeof(int)));
  gr_block_sptr head (gr_make_head(sizeof(int), N));
  gr_annotator_1to1_sptr ann0 (gr_make_annotator_1to1(10000, sizeof(int)));
  gr_annotator_1to1_sptr ann1 (gr_make_annotator_1to1(10000, sizeof(int)));
  gr_annotator_1to1_sptr ann2 (gr_make_annotator_1to1(10000, sizeof(int)));
  gr_block_sptr snk0 (gr_make_null_sink(sizeof(int)));
  gr_block_sptr snk1 (gr_make_null_sink(sizeof(int)));

  // using 1-to-1 tag propagation without having equal number of
  // ins and outs. Make sure this works; will just exit run early.
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, ann0, 0);
  tb->connect(ann0, 0, ann1, 0);
  tb->connect(ann0, 1, ann2, 0);
  tb->connect(ann1, 0, snk0, 0);
  tb->connect(ann2, 0, snk1, 0);

  std::cerr << std::endl
	    << "NOTE: This is supposed to produce an error from gr_block_executor"
	    << std::endl;
  tb->run();
}


void
qa_block_tags::t5 ()
{
  int N = 40000;
  gr_top_block_sptr tb = gr_make_top_block("top");
  gr_block_sptr src (gr_make_null_source(sizeof(float)));
  gr_block_sptr head (gr_make_head(sizeof(float), N));
  gr_annotator_alltoall_sptr ann0 (gr_make_annotator_alltoall(10000, sizeof(float)));
  gr_annotator_alltoall_sptr ann1 (gr_make_annotator_alltoall(10000, sizeof(float)));
  gr_annotator_alltoall_sptr ann2 (gr_make_annotator_alltoall(1000,  sizeof(float)));
  gr_block_sptr snk0 (gr_make_null_sink(sizeof(float)));

  // Rate change blocks
  gr_keep_one_in_n_sptr dec10 (gr_make_keep_one_in_n(sizeof(float), 10));

  tb->connect(src,  0, head, 0);
  tb->connect(head, 0, ann0, 0);
  tb->connect(ann0, 0, ann1, 0);
  tb->connect(ann1, 0, dec10, 0);
  tb->connect(dec10, 0, ann2, 0);
  tb->connect(ann2, 0, snk0, 0);

  tb->run();

  std::vector<gr_tag_t> tags0 = ann0->data();
  std::vector<gr_tag_t> tags1 = ann1->data();
  std::vector<gr_tag_t> tags2 = ann2->data();

  // The first annotator does not receive any tags from the null sink upstream
  CPPUNIT_ASSERT_EQUAL(tags0.size(), (size_t)0);
  CPPUNIT_ASSERT_EQUAL(tags1.size(), (size_t)4);
  CPPUNIT_ASSERT_EQUAL(tags2.size(), (size_t)8);


#if QA_TAGS_DEBUG
  // Kludge together the tags that we know should result from the above graph
  std::stringstream str0, str1, str2;
  str0 << ann0->name() << ann0->unique_id();
  str1 << ann1->name() << ann1->unique_id();
  str2 << ann2->name() << ann2->unique_id();

  pmt_t expected_tags1[5];
  expected_tags1[0] = mp(pmt_from_uint64(0),     mp(str0.str()), mp("seq"), mp(0));
  expected_tags1[1] = mp(pmt_from_uint64(10000), mp(str0.str()), mp("seq"), mp(1));
  expected_tags1[2] = mp(pmt_from_uint64(20000), mp(str0.str()), mp("seq"), mp(2));
  expected_tags1[3] = mp(pmt_from_uint64(30000), mp(str0.str()), mp("seq"), mp(3));

  pmt_t expected_tags2[10];
  expected_tags2[0] = mp(pmt_from_uint64(0),    mp(str1.str()), mp("seq"), mp(0));
  expected_tags2[1] = mp(pmt_from_uint64(0),    mp(str0.str()), mp("seq"), mp(0));
  expected_tags2[2] = mp(pmt_from_uint64(1000), mp(str1.str()), mp("seq"), mp(1));
  expected_tags2[3] = mp(pmt_from_uint64(1000), mp(str0.str()), mp("seq"), mp(1));
  expected_tags2[4] = mp(pmt_from_uint64(2000), mp(str1.str()), mp("seq"), mp(2));
  expected_tags2[5] = mp(pmt_from_uint64(2000), mp(str0.str()), mp("seq"), mp(2));
  expected_tags2[6] = mp(pmt_from_uint64(3000), mp(str1.str()), mp("seq"), mp(3));
  expected_tags2[7] = mp(pmt_from_uint64(3000), mp(str0.str()), mp("seq"), mp(3));
  expected_tags2[8] = mp(pmt_from_uint64(4000), mp(str1.str()), mp("seq"), mp(4));
  expected_tags2[9] = mp(pmt_from_uint64(4000), mp(str0.str()), mp("seq"), mp(4));

  std::cout << std::endl << "qa_block_tags::t5" << std::endl;

  // annotator 1 gets tags from annotator 0
  std::cout << "tags1.size(): " << tags1.size() << std::endl;
  for(size_t i = 0; i < tags1.size(); i++) {
    std::cout << "tags1[" << i << "] = " << tags1[i] << "\t\t" << expected_tags1[i] << std::endl;
    CPPUNIT_ASSERT_EQUAL(pmt_write_string(tags1[i]), pmt_write_string(expected_tags1[i]));
  }

  // annotator 2 gets tags from annotators 0 and 1
  std::cout << std::endl;
  std::cout << "tags2.size(): " << tags2.size() << std::endl;
  for(size_t i = 0; i < tags2.size(); i++) {
    std::cout << "tags2[" << i << "] = " << tags2[i] << "\t\t" << expected_tags2[i] << std::endl;
    CPPUNIT_ASSERT_EQUAL(pmt_write_string(tags2[i]), pmt_write_string(expected_tags2[i]));
  }
#endif
}


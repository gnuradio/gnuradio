/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/block.h>
#include <gnuradio/blocks/annotator_1to1.h>
#include <gnuradio/blocks/annotator_alltoall.h>
#include <gnuradio/blocks/head.h>
#include <gnuradio/blocks/keep_one_in_n.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/logger.h>
#include <gnuradio/top_block.h>

#include <boost/test/unit_test.hpp>

#include <memory>

// ----------------------------------------------------------------

// set to 1 to turn on debug output
// The debug output fully checks that the tags seen are what are expected. While
// this behavior currently works with our implementation, there is no guarantee
// that the tags will be coming in this specific order, so it's dangerous to
// rely on this as a test of the tag system working. We would really want to
// tags we know we should see and then test that they all occur once, but in no
// particular order.
#define QA_TAGS_DEBUG 0

gr::tag_t make_tag(uint64_t offset, pmt::pmt_t key, pmt::pmt_t value, pmt::pmt_t srcid)
{
    gr::tag_t result;
    result.offset = offset;
    result.key = key;
    result.value = value;
    result.srcid = srcid;
    return result;
}

std::ostream& operator<<(std::ostream& os, const gr::tag_t& t) { return os; }

BOOST_AUTO_TEST_CASE(t0)
{
    unsigned int N = 1000;
    gr::top_block_sptr tb = gr::make_top_block("top");
    gr::block_sptr src(gr::blocks::null_source::make(sizeof(int)));
    gr::block_sptr head(gr::blocks::head::make(sizeof(int), N));
    gr::block_sptr snk(gr::blocks::null_sink::make(sizeof(int)));

    tb->connect(src, 0, head, 0);
    tb->connect(head, 0, snk, 0);

    BOOST_REQUIRE_EQUAL(src->nitems_read(0), (uint64_t)0);
    BOOST_REQUIRE_EQUAL(src->nitems_written(0), (uint64_t)0);

    tb->run();

    BOOST_REQUIRE_THROW(src->nitems_read(0), std::invalid_argument);
    BOOST_REQUIRE(src->nitems_written(0) >= N);
    BOOST_REQUIRE_EQUAL(snk->nitems_read(0), (uint64_t)1000);
    BOOST_REQUIRE_THROW(snk->nitems_written(0), std::invalid_argument);
}


BOOST_AUTO_TEST_CASE(t1)
{
    int N = 40000;
    gr::top_block_sptr tb = gr::make_top_block("top");
    gr::block_sptr src(gr::blocks::null_source::make(sizeof(int)));
    gr::block_sptr head(gr::blocks::head::make(sizeof(int), N));
    gr::blocks::annotator_alltoall::sptr ann0(
        gr::blocks::annotator_alltoall::make(10000, sizeof(int)));
    gr::blocks::annotator_alltoall::sptr ann1(
        gr::blocks::annotator_alltoall::make(10000, sizeof(int)));
    gr::blocks::annotator_alltoall::sptr ann2(
        gr::blocks::annotator_alltoall::make(10000, sizeof(int)));
    gr::blocks::annotator_alltoall::sptr ann3(
        gr::blocks::annotator_alltoall::make(10000, sizeof(int)));
    gr::blocks::annotator_alltoall::sptr ann4(
        gr::blocks::annotator_alltoall::make(10000, sizeof(int)));
    gr::block_sptr snk0(gr::blocks::null_sink::make(sizeof(int)));
    gr::block_sptr snk1(gr::blocks::null_sink::make(sizeof(int)));

    tb->connect(src, 0, head, 0);
    tb->connect(head, 0, ann0, 0);

    tb->connect(ann0, 0, ann1, 0);
    tb->connect(ann0, 1, ann2, 0);
    tb->connect(ann1, 0, ann3, 0);
    tb->connect(ann2, 0, ann4, 0);

    tb->connect(ann3, 0, snk0, 0);
    tb->connect(ann4, 0, snk1, 0);

    tb->run();

    std::vector<gr::tag_t> tags0 = ann0->data();
    std::vector<gr::tag_t> tags3 = ann3->data();
    std::vector<gr::tag_t> tags4 = ann4->data();

    // The first annotator does not receive any tags from the null sink upstream
    BOOST_REQUIRE_EQUAL(tags0.size(), (size_t)0);
    BOOST_REQUIRE_EQUAL(tags3.size(), (size_t)8);
    BOOST_REQUIRE_EQUAL(tags4.size(), (size_t)8);

#if QA_TAGS_DEBUG
    auto logger = std::make_shared<gr::logger>("qa_block_tags::t1");

    // Kludge together the tags that we know should result from the above graph
    std::stringstream str0, str1, str2;
    str0 << ann0->name() << ann0->unique_id();
    str1 << ann1->name() << ann1->unique_id();
    str2 << ann2->name() << ann2->unique_id();

    gr::tag_t expected_tags3[8];
    expected_tags3[0] = make_tag(0, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(0));
    expected_tags3[1] = make_tag(0, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(0));
    expected_tags3[2] = make_tag(10000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(1));
    expected_tags3[3] = make_tag(10000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(2));
    expected_tags3[4] = make_tag(20000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(2));
    expected_tags3[5] = make_tag(20000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(4));
    expected_tags3[6] = make_tag(30000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(3));
    expected_tags3[7] = make_tag(30000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(6));

    gr::tag_t expected_tags4[8];
    expected_tags4[0] = make_tag(0, pmt::mp(str2.str()), pmt::mp("seq"), pmt::mp(0));
    expected_tags4[1] = make_tag(0, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(1));
    expected_tags4[2] = make_tag(10000, pmt::mp(str2.str()), pmt::mp("seq"), pmt::mp(1));
    expected_tags4[3] = make_tag(10000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(3));
    expected_tags4[4] = make_tag(20000, pmt::mp(str2.str()), pmt::mp("seq"), pmt::mp(2));
    expected_tags4[5] = make_tag(20000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(5));
    expected_tags4[6] = make_tag(30000, pmt::mp(str2.str()), pmt::mp("seq"), pmt::mp(3));
    expected_tags4[7] = make_tag(30000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(7));

    // For annotator 3, we know it gets tags from ann0 and ann1, test this
    for (size_t i = 0; i < tags3.size(); i++) {
        logger->info("tags3[{:d}] = {:16s}{:16s}", i, tags3[i], expected_tags3[i]);
        BOOST_REQUIRE_EQUAL(tags3[i], expected_tags3[i]);
    }

    // For annotator 4, we know it gets tags from ann0 and ann2, test this
    for (size_t i = 0; i < tags4.size(); i++) {
        logger->info("tags4[{:d}] = {:16s}{:16s}", i, tags4[i], expected_tags4[i]);
        BOOST_REQUIRE_EQUAL(tags4[i], expected_tags4[i]);
    }
#endif
}

BOOST_AUTO_TEST_CASE(t2)
{
    int N = 40000;
    gr::top_block_sptr tb = gr::make_top_block("top");
    gr::block_sptr src(gr::blocks::null_source::make(sizeof(int)));
    gr::block_sptr head(gr::blocks::head::make(sizeof(int), N));
    gr::blocks::annotator_alltoall::sptr ann0(
        gr::blocks::annotator_alltoall::make(10000, sizeof(int)));
    gr::blocks::annotator_alltoall::sptr ann1(
        gr::blocks::annotator_alltoall::make(10000, sizeof(int)));
    gr::blocks::annotator_alltoall::sptr ann2(
        gr::blocks::annotator_alltoall::make(10000, sizeof(int)));
    gr::blocks::annotator_alltoall::sptr ann3(
        gr::blocks::annotator_alltoall::make(10000, sizeof(int)));
    gr::blocks::annotator_alltoall::sptr ann4(
        gr::blocks::annotator_alltoall::make(10000, sizeof(int)));
    gr::block_sptr snk0(gr::blocks::null_sink::make(sizeof(int)));
    gr::block_sptr snk1(gr::blocks::null_sink::make(sizeof(int)));
    gr::block_sptr snk2(gr::blocks::null_sink::make(sizeof(int)));

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

    std::vector<gr::tag_t> tags0 = ann0->data();
    std::vector<gr::tag_t> tags1 = ann1->data();
    std::vector<gr::tag_t> tags2 = ann2->data();
    std::vector<gr::tag_t> tags3 = ann4->data();
    std::vector<gr::tag_t> tags4 = ann4->data();

    // The first annotator does not receive any tags from the null sink upstream
    BOOST_REQUIRE_EQUAL(tags0.size(), (size_t)0);
    BOOST_REQUIRE_EQUAL(tags1.size(), (size_t)8);

    // Make sure the rest all have 12 tags
    BOOST_REQUIRE_EQUAL(tags2.size(), (size_t)12);
    BOOST_REQUIRE_EQUAL(tags3.size(), (size_t)12);
    BOOST_REQUIRE_EQUAL(tags4.size(), (size_t)12);


#if QA_TAGS_DEBUG
    auto logger = std::make_shared<gr::logger>("qa_block_tags::t2");

    // Kludge together the tags that we know should result from the above graph
    std::stringstream str0, str1;
    str0 << ann0->name() << ann0->unique_id();
    str1 << ann1->name() << ann1->unique_id();

    gr::tag_t expected_tags2[12];
    expected_tags2[0] = make_tag(0, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(0));
    expected_tags2[1] = make_tag(0, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(0));
    expected_tags2[2] = make_tag(0, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(1));
    expected_tags2[3] = make_tag(10000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(3));
    expected_tags2[4] = make_tag(10000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(2));
    expected_tags2[5] = make_tag(10000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(3));
    expected_tags2[6] = make_tag(20000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(6));
    expected_tags2[7] = make_tag(20000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(4));
    expected_tags2[8] = make_tag(20000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(5));
    expected_tags2[9] = make_tag(30000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(9));
    expected_tags2[10] = make_tag(30000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(6));
    expected_tags2[11] = make_tag(30000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(7));

    gr::tag_t expected_tags4[12];
    expected_tags4[0] = make_tag(0, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(2));
    expected_tags4[1] = make_tag(0, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(0));
    expected_tags4[2] = make_tag(0, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(1));
    expected_tags4[3] = make_tag(10000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(5));
    expected_tags4[4] = make_tag(10000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(2));
    expected_tags4[5] = make_tag(10000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(3));
    expected_tags4[6] = make_tag(20000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(8));
    expected_tags4[7] = make_tag(20000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(4));
    expected_tags4[8] = make_tag(20000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(5));
    expected_tags4[9] = make_tag(30000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(11));
    expected_tags4[10] = make_tag(30000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(6));
    expected_tags4[11] = make_tag(30000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(7));

    // For annotator[2-4], we know it gets tags from ann0 and ann1
    // but the tags from the different outputs of ann1 are different for each.
    // Just testing ann2 and ann4; if they are correct it would be
    // inconceivable for ann3 to have it wrong.
    for (size_t i = 0; i < tags2.size(); i++) {
        logger->info("tags2[{:d}] = {:16s}{:16s}", i, tags2[i], expected_tags2[i]);
        BOOST_REQUIRE_EQUAL(tags2[i], expected_tags2[i]);
    }

    for (size_t i = 0; i < tags4.size(); i++) {
        logger->info("tags4[{:d}] = {:16s}{:16s}", i, tags4[i], expected_tags4[i]);
        BOOST_REQUIRE_EQUAL(tags4[i], expected_tags4[i]);
    }
#endif
}


BOOST_AUTO_TEST_CASE(t3)
{
    int N = 40000;
    gr::top_block_sptr tb = gr::make_top_block("top");
    gr::block_sptr src(gr::blocks::null_source::make(sizeof(int)));
    gr::block_sptr head(gr::blocks::head::make(sizeof(int), N));
    gr::blocks::annotator_1to1::sptr ann0(
        gr::blocks::annotator_1to1::make(10000, sizeof(int)));
    gr::blocks::annotator_alltoall::sptr ann1(
        gr::blocks::annotator_alltoall::make(10000, sizeof(int)));
    gr::blocks::annotator_alltoall::sptr ann2(
        gr::blocks::annotator_alltoall::make(10000, sizeof(int)));
    gr::blocks::annotator_1to1::sptr ann3(
        gr::blocks::annotator_1to1::make(10000, sizeof(int)));
    gr::blocks::annotator_1to1::sptr ann4(
        gr::blocks::annotator_1to1::make(10000, sizeof(int)));
    gr::block_sptr snk0(gr::blocks::null_sink::make(sizeof(int)));
    gr::block_sptr snk1(gr::blocks::null_sink::make(sizeof(int)));

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


    std::vector<gr::tag_t> tags0 = ann0->data();
    std::vector<gr::tag_t> tags3 = ann3->data();
    std::vector<gr::tag_t> tags4 = ann4->data();

    // The first annotator does not receive any tags from the null sink upstream
    BOOST_REQUIRE_EQUAL(tags0.size(), (size_t)0);
    BOOST_REQUIRE_EQUAL(tags3.size(), (size_t)8);
    BOOST_REQUIRE_EQUAL(tags4.size(), (size_t)8);

#if QA_TAGS_DEBUG
    auto logger = std::make_shared<gr::logger>("qa_block_tags::t3");

    // Kludge together the tags that we know should result from the above graph
    std::stringstream str0, str1, str2;
    str0 << ann0->name() << ann0->unique_id();
    str1 << ann1->name() << ann1->unique_id();
    str2 << ann2->name() << ann2->unique_id();

    gr::tag_t expected_tags3[8];
    expected_tags3[0] = make_tag(0, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(0));
    expected_tags3[1] = make_tag(0, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(0));
    expected_tags3[2] = make_tag(10000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(1));
    expected_tags3[3] = make_tag(10000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(2));
    expected_tags3[4] = make_tag(20000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(2));
    expected_tags3[5] = make_tag(20000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(4));
    expected_tags3[6] = make_tag(30000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(3));
    expected_tags3[7] = make_tag(30000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(6));

    gr::tag_t expected_tags4[8];
    expected_tags4[0] = make_tag(0, pmt::mp(str2.str()), pmt::mp("seq"), pmt::mp(0));
    expected_tags4[1] = make_tag(0, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(1));
    expected_tags4[2] = make_tag(10000, pmt::mp(str2.str()), pmt::mp("seq"), pmt::mp(1));
    expected_tags4[3] = make_tag(10000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(3));
    expected_tags4[4] = make_tag(20000, pmt::mp(str2.str()), pmt::mp("seq"), pmt::mp(2));
    expected_tags4[5] = make_tag(20000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(5));
    expected_tags4[6] = make_tag(30000, pmt::mp(str2.str()), pmt::mp("seq"), pmt::mp(3));
    expected_tags4[7] = make_tag(30000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(7));

    // For annotator 3, we know it gets tags from ann0 and ann1, test this
    for (size_t i = 0; i < tags3.size(); i++) {
        logger->info("tags4[{:d}] = {:16s}{:16s}", i, tags4[i], expected_tags4[i]);
        BOOST_REQUIRE_EQUAL(tags3[i], expected_tags3[i]);
    }

    // For annotator 4, we know it gets tags from ann0 and ann2, test this
    for (size_t i = 0; i < tags4.size(); i++) {
        logger->info("tags4[{:d}] = {:16s}{:16s}", i, tags4[i], expected_tags4[i]);
        BOOST_REQUIRE_EQUAL(tags4[i], expected_tags4[i]);
    }
#endif
}


BOOST_AUTO_TEST_CASE(t4)
{
    int N = 40000;
    gr::top_block_sptr tb = gr::make_top_block("top");
    gr::block_sptr src(gr::blocks::null_source::make(sizeof(int)));
    gr::block_sptr head(gr::blocks::head::make(sizeof(int), N));
    gr::blocks::annotator_1to1::sptr ann0(
        gr::blocks::annotator_1to1::make(10000, sizeof(int)));
    gr::blocks::annotator_1to1::sptr ann1(
        gr::blocks::annotator_1to1::make(10000, sizeof(int)));
    gr::blocks::annotator_1to1::sptr ann2(
        gr::blocks::annotator_1to1::make(10000, sizeof(int)));
    gr::block_sptr snk0(gr::blocks::null_sink::make(sizeof(int)));
    gr::block_sptr snk1(gr::blocks::null_sink::make(sizeof(int)));

    auto logger = std::make_shared<gr::logger>("qa_block_tags::t4");

    // using 1-to-1 tag propagation without having equal number of
    // ins and outs. Make sure this works; will just exit run early.
    tb->connect(src, 0, head, 0);
    tb->connect(head, 0, ann0, 0);
    tb->connect(ann0, 0, ann1, 0);
    tb->connect(ann0, 1, ann2, 0);
    tb->connect(ann1, 0, snk0, 0);
    tb->connect(ann2, 0, snk1, 0);

    logger->info("NOTE: This is supposed to produce an error from block_executor!");
    tb->run();
}


BOOST_AUTO_TEST_CASE(t5)
{
    int N = 40000;
    gr::top_block_sptr tb = gr::make_top_block("top");
    gr::block_sptr src(gr::blocks::null_source::make(sizeof(float)));
    gr::block_sptr head(gr::blocks::head::make(sizeof(float), N));
    gr::blocks::annotator_alltoall::sptr ann0(
        gr::blocks::annotator_alltoall::make(10000, sizeof(float)));
    gr::blocks::annotator_alltoall::sptr ann1(
        gr::blocks::annotator_alltoall::make(10000, sizeof(float)));
    gr::blocks::annotator_alltoall::sptr ann2(
        gr::blocks::annotator_alltoall::make(1000, sizeof(float)));
    gr::block_sptr snk0(gr::blocks::null_sink::make(sizeof(float)));

    // Rate change blocks
    gr::blocks::keep_one_in_n::sptr dec10(
        gr::blocks::keep_one_in_n::make(sizeof(float), 10));

    tb->connect(src, 0, head, 0);
    tb->connect(head, 0, ann0, 0);
    tb->connect(ann0, 0, ann1, 0);
    tb->connect(ann1, 0, dec10, 0);
    tb->connect(dec10, 0, ann2, 0);
    tb->connect(ann2, 0, snk0, 0);

    tb->run();

    std::vector<gr::tag_t> tags0 = ann0->data();
    std::vector<gr::tag_t> tags1 = ann1->data();
    std::vector<gr::tag_t> tags2 = ann2->data();

    // The first annotator does not receive any tags from the null sink upstream
    BOOST_REQUIRE_EQUAL(tags0.size(), (size_t)0);
    BOOST_REQUIRE_EQUAL(tags1.size(), (size_t)4);
    BOOST_REQUIRE_EQUAL(tags2.size(), (size_t)8);


#if QA_TAGS_DEBUG
    auto logger = std::make_shared<gr::logger>("qa_block_tags::t5");

    // Kludge together the tags that we know should result from the above graph
    std::stringstream str0, str1, str2;
    str0 << ann0->name() << ann0->unique_id();
    str1 << ann1->name() << ann1->unique_id();
    str2 << ann2->name() << ann2->unique_id();

    gr::tag_t expected_tags1[5];
    expected_tags1[0] = make_tag(0, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(0));
    expected_tags1[1] = make_tag(10000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(1));
    expected_tags1[2] = make_tag(20000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(2));
    expected_tags1[3] = make_tag(30000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(3));

    gr::tag_t expected_tags2[10];
    expected_tags2[0] = make_tag(0, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(0));
    expected_tags2[1] = make_tag(0, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(0));
    expected_tags2[2] = make_tag(1000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(1));
    expected_tags2[3] = make_tag(1000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(1));
    expected_tags2[4] = make_tag(2000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(2));
    expected_tags2[5] = make_tag(2000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(2));
    expected_tags2[6] = make_tag(3000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(3));
    expected_tags2[7] = make_tag(3000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(3));
    expected_tags2[8] = make_tag(4000, pmt::mp(str1.str()), pmt::mp("seq"), pmt::mp(4));
    expected_tags2[9] = make_tag(4000, pmt::mp(str0.str()), pmt::mp("seq"), pmt::mp(4));

    logger->info("tags1.size(): {:d}", tags1.size);

    // annotator 1 gets tags from annotator 0
    for (size_t i = 0; i < tags1.size(); i++) {
        logger->info("tags1[{:d}] = {:16s}{:16s}", i, tags1[i], expected_tags1[i]);
        BOOST_REQUIRE_EQUAL(tags1[i], expected_tags1[i]);
    }

    // annotator 2 gets tags from annotators 0 and 1
    logger->info("tags2.size(): {:d}", tags2.size);
    for (size_t i = 0; i < tags2.size(); i++) {
        logger->info("tags2[{:d}] = {:16s}{:16s}", i, tags2[i], expected_tags2[i]);
        BOOST_REQUIRE_EQUAL(tags2[i], expected_tags2[i]);
    }
#endif
}

/* -*- c++ -*- */
/*
 * Copyright 2023 Ettus Research, A National Instruments Brand
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/blocks/lambda_block.h>
#include <gnuradio/blocks/vector_sink.h>
#include <gnuradio/blocks/vector_source.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/top_block.h>
#include <boost/test/unit_test.hpp>

#define VERBOSE 0

BOOST_AUTO_TEST_CASE(lambda_block_run)
{
    auto tb = gr::make_top_block("top");

    if (VERBOSE)
        std::cout << "[lambda_block_run] Setting up blocks..." << std::endl;

    auto src = gr::blocks::vector_source_f::make({ 0.0f, 1.0f, 2.0f, 4.0f });
    auto lambda = gr::blocks::lambda_block::make(
        [](gr::blocks::lambda_block* self,
           int noutput_items,
           gr_vector_int& ninput_items,
           gr_vector_const_void_star& input_items,
           gr_vector_void_star& output_items) -> int {
            const float* in = static_cast<const float*>(input_items[0]);
            float* out = static_cast<float*>(output_items[0]);
            const size_t items_to_process =
                static_cast<size_t>(std::min(noutput_items, ninput_items[0]));
            if (VERBOSE)
                std::cout << "[lambda_block_run] general_work(): Items to process: "
                          << items_to_process << std::endl;
            for (size_t i; i < items_to_process; i++) {
                out[i] = 2 * in[i];
            }
            self->consume_each(items_to_process);
            self->add_item_tag(0, 0, pmt::string_to_symbol("test"), pmt::PMT_T);
            return static_cast<int>(items_to_process);
        },
        gr::io_signature::make(1, 1, sizeof(float)),
        gr::io_signature::make(1, 1, sizeof(float)),
        "my_doubler");

    auto dst = gr::blocks::vector_sink_f::make();

    if (VERBOSE)
        std::cout << "[lambda_block_run] Connecting blocks..." << std::endl;
    tb->connect(src, 0, lambda, 0);
    tb->connect(lambda, 0, dst, 0);
    if (VERBOSE)
        std::cout << "[lambda_block_run] Running flow graph..." << std::endl;
    tb->run();

    const std::vector<float> expected{ 0.0f, 2.0f, 4.0f, 8.0f };
    BOOST_TEST(dst->data() == expected, boost::test_tools::per_element());
    auto tags = dst->tags();
    BOOST_CHECK_EQUAL(tags.size(), 1);
    BOOST_CHECK_EQUAL(tags[0].offset, 0);
    BOOST_CHECK_EQUAL(tags[0].key, pmt::string_to_symbol("test"));
}

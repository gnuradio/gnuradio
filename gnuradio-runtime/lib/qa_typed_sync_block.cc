/* -*- c++ -*- */
/*
 * Copyright 2021 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include <gnuradio/top_block.h>
#include <gnuradio/typed_sync_block.h>
#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <memory>
using gr::produced_t;

using super = gr::typed_sync_block<int, float>;
class int_to_float : public super
{

public:
    using sptr = std::shared_ptr<int_to_float>;
    static sptr make() { return gnuradio::make_block_sptr<int_to_float>(); }

    produced_t
    work(size_t n_items, inputs_t input_streams, outputs_t output_streams) override
    {
        auto& out = output_streams[0];
        const auto& in = input_streams[0];
        for (size_t idx = 0; idx < n_items; ++idx) {
            out[idx] = static_cast<out_type>(in[idx]);
        }
        return { n_items };
    }
    int_to_float() : super("int_to_float") {}
};

using int_src = gr::typed_sync_block<char, int>;
class int_generator : public int_src
{
private:
    size_t _counter = 0;

public:
    using sptr = std::shared_ptr<int_generator>;
    static sptr make() { return gnuradio::make_block_sptr<int_generator>(); }

    produced_t
    work(size_t n_items, inputs_t input_streams, outputs_t output_streams) override
    {
        for (const auto& output_stream_span : output_streams) {
            for (auto& item : output_stream_span) {
                item = _counter++ & ((1 << 12) - 1);
            }
        }
        return { n_items };
    }
    int_generator() : int_src("int_generator", 0, 0) {}
};

template <typename value_t>
using typed_sink = gr::typed_sync_block<value_t, char>;

template <typename value_t>
class vector_snk : public typed_sink<value_t>
{
private:
    size_t _left;
    std::vector<value_t> _vector;

public:
    using sptr = std::shared_ptr<vector_snk<value_t>>;
    static sptr make(size_t total = 1000000)
    {
        return gnuradio::make_block_sptr<vector_snk<value_t>>(total);
    }

    produced_t work(size_t n_items,
                    typename typed_sink<value_t>::inputs_t input_streams,
                    typename typed_sink<value_t>::outputs_t output_streams) override
    {
        size_t to_copy = std::min(_left, n_items);
        const auto& stream = input_streams[0];
        std::copy(stream, stream + to_copy, std::back_inserter(_vector));
        _left -= to_copy;
        return { to_copy, !(_left) };
    }
    vector_snk(size_t total = 100000)
        : typed_sink<value_t>("typed_sink", 1, 1, 0, 0), _left(total)
    {
        _vector.reserve(total);
    }
    const std::vector<value_t>& data() const { return _vector; }
};

BOOST_AUTO_TEST_CASE(instantiate_blocks)
{
    auto block_ptr_i2f = int_to_float::make();
    auto block_ptr_src = int_generator::make();
    auto block_ptr_sink = vector_snk<float>::make();
}

BOOST_AUTO_TEST_CASE(flowgraph)
{
    auto i2f = int_to_float::make();
    auto src = int_generator::make();
    auto sink = vector_snk<float>::make();
    auto tb = gr::make_top_block("end-to-end");
    tb->connect(src, 0, i2f, 0);
    tb->connect(i2f, 0, sink, 0);
    tb->run();

    const auto& data = sink->data();
    size_t counter = 0;
    for (const auto& sample : data) {
        BOOST_CHECK(size_t(sample) == counter);
        counter = (counter + 1) & ((1 << 12) - 1);
    }
}

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

#include <gnuradio/blocks/vector_sink.h>
#include <gnuradio/blocks/vector_source.h>
#include <gnuradio/buffer_cpu_vmcirc.h>
#include <gnuradio/flowgraph.h>
#include <gnuradio/math/multiply_const.h>
#include <gnuradio/runtime.h>
#include <gnuradio/schedulers/nbt/scheduler_nbt.h>
#include <gnuradio/streamops/copy.h>

using namespace gr;

TEST(SchedulerMTTest, TwoSinks)
{
    int nsamples = 100000;
    std::vector<float> input_data(nsamples);
    for (int i = 0; i < nsamples; i++) {
        input_data[i] = i;
    }
    auto src = blocks::vector_source_f::make_cpu({ input_data, false });
    auto snk1 = blocks::vector_sink_f::make({});
    auto snk2 = blocks::vector_sink_f::make({});


    auto fg = flowgraph::make();
    fg->connect(src, 0, snk1, 0);
    fg->connect(src, 0, snk2, 0);

    auto rt = runtime::make();
    rt->initialize(fg);
    rt->start();
    rt->wait();

    EXPECT_EQ(snk1->data().size(), input_data.size());
    EXPECT_EQ(snk2->data().size(), input_data.size());
    EXPECT_EQ(snk1->data(), input_data);
    EXPECT_EQ(snk2->data(), input_data);
}

TEST(SchedulerMTTest, MultiDomainBasic)
{
    std::vector<float> input_data{ 1.0, 2.0, 3.0, 4.0, 5.0 };

    std::vector<float> expected_data;
    for (auto d : input_data) {
        expected_data.push_back(100.0 * 200.0 * d);
    }

    auto src = blocks::vector_source_f::make_cpu({ input_data, false });
    auto mult1 = math::multiply_const_ff::make_cpu({ 100.0 });
    auto mult2 = math::multiply_const_ff::make_cpu({ 200.0 });
    auto snk = blocks::vector_sink_f::make({});

    flowgraph_sptr fg(new flowgraph());
    fg->connect(src, mult1);
    fg->connect(mult1, mult2);
    fg->connect(mult2, snk);

    auto sched1 = schedulers::scheduler_nbt::make("sched1");
    auto sched2 = schedulers::scheduler_nbt::make("sched2");

    auto rt = runtime::make();
    rt->add_scheduler({ sched1, { src, mult1 } });
    rt->add_scheduler({ sched2, { mult2, snk } });
    rt->initialize(fg);
    rt->start();
    rt->wait();

    EXPECT_EQ(snk->data(), expected_data);
}

TEST(SchedulerMTTest, BlockFanout)
{
    int nsamples = 1000000;
    std::vector<gr_complex> input_data(nsamples);
    std::vector<gr_complex> expected_data(nsamples);
    int buffer_type = 0;
    float k = 1.0;
    for (int i = 0; i < nsamples; i++) {
        input_data[i] = gr_complex(2 * i, 2 * i + 1);
        // expected_output[i] = gr_complex(k*2*i,k*2*i+1);
    }

    for (auto nblocks : { 2, 8, 16 }) {
        // for (auto nblocks : { 2, }) {
        size_t veclen = 1;
        auto src = blocks::vector_source_c::make_cpu({ input_data });
        std::vector<blocks::vector_sink_c::sptr> sink_blks(nblocks);
        std::vector<math::multiply_const_cc::sptr> mult_blks(nblocks);

        for (int i = 0; i < nblocks; i++) {
            mult_blks[i] = math::multiply_const_cc::make_cpu({ k, veclen });
            sink_blks[i] = blocks::vector_sink_c::make({});
        }
        flowgraph_sptr fg(new flowgraph());

        if (buffer_type == 0) {
            for (int i = 0; i < nblocks; i++) {
                fg->connect(src, 0, mult_blks[i], 0);
                fg->connect(mult_blks[i], 0, sink_blks[i], 0);
            }
        }
        else {
            for (int i = 0; i < nblocks; i++) {
                fg->connect(src, 0, mult_blks[i], 0)
                    ->set_custom_buffer(BUFFER_CPU_VMCIRC_ARGS);
                fg->connect(mult_blks[i], 0, sink_blks[i], 0)
                    ->set_custom_buffer(BUFFER_CPU_VMCIRC_ARGS);
            }
        }

        auto rt = runtime::make();
        rt->initialize(fg);
        rt->start();
        rt->wait();

        for (int n = 0; n < nblocks; n++) {
            for (int i = 0; i < nsamples; i++) {
                input_data[i] = gr_complex(2 * i, 2 * i + 1);
                expected_data[i] = gr_complex(k * 2 * i, k * (2 * i + 1));
            }

            EXPECT_EQ(sink_blks[n]->data(), expected_data);
            EXPECT_EQ(sink_blks[n]->data().size(), expected_data.size());
        }
    }
}

TEST(SchedulerMTTest, CustomCPUBuffers)
{
    int nsamples = 100000;
    std::vector<float> input_data(nsamples);
    for (int i = 0; i < nsamples; i++) {
        input_data[i] = i;
    }
    auto src = blocks::vector_source_f::make({ input_data, false });
    auto copy1 = streamops::copy::make({ sizeof(float) });
    auto copy2 = streamops::copy::make({ sizeof(float) });
    auto copy3 = streamops::copy::make({ sizeof(float) });
    auto snk1 = blocks::vector_sink_f::make({});
    auto snk2 = blocks::vector_sink_f::make({});


    flowgraph_sptr fg(new flowgraph());
    fg->connect(src, 0, copy1, 0);
    fg->connect(copy1, 0, copy2, 0)
        ->set_custom_buffer(
            buffer_cpu_vmcirc_properties::make(buffer_cpu_vmcirc_type::AUTO)
                ->set_buffer_size(4096));
    fg->connect(copy2, 0, snk1, 0)
        ->set_custom_buffer(
            buffer_cpu_vmcirc_properties::make(buffer_cpu_vmcirc_type::AUTO)
                ->set_min_buffer_size(4096)
                ->set_max_buffer_size(8192));
    fg->connect(copy1, 0, copy3, 0)
        ->set_custom_buffer(
            buffer_cpu_vmcirc_properties::make(buffer_cpu_vmcirc_type::AUTO)
                ->set_buffer_size(16384));
    fg->connect(copy3, 0, snk2, 0)
        ->set_custom_buffer(
            buffer_cpu_vmcirc_properties::make(buffer_cpu_vmcirc_type::AUTO)
                ->set_min_buffer_size(16384));

    // TODO: Validate the buffers that were created

    auto rt = runtime::make();
    rt->initialize(fg);
    rt->start();
    rt->wait();

    EXPECT_EQ(snk1->data().size(), input_data.size());
    EXPECT_EQ(snk2->data().size(), input_data.size());
    EXPECT_EQ(snk1->data(), input_data);
    EXPECT_EQ(snk2->data(), input_data);
}

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

#include <gnuradio/blocks/vector_sink.h>
#include <gnuradio/blocks/vector_source.h>
#include <gnuradio/buffer_sm.h>
#include <gnuradio/flowgraph.h>
#include <gnuradio/math/multiply_const.h>
#include <gnuradio/runtime.h>
#include <gnuradio/schedulers/nbt/scheduler_nbt.h>

using namespace gr;


TEST(SchedulerMTSingleBuffers, SingleMappedSimple)
{
    int nsamples = 1000000;
    std::vector<gr_complex> input_data(nsamples);
    std::vector<gr_complex> expected_data(nsamples);

    float k = 1.0;
    for (int i = 0; i < nsamples; i++) {
        input_data[i] = gr_complex(2 * i, 2 * i + 1);
    }

    int nblocks = 2;
    size_t veclen = 1;
    auto src = blocks::vector_source_c::make({ input_data });
    auto snk = blocks::vector_sink_c::make({});
    std::vector<math::multiply_const_cc::sptr> mult_blks(nblocks);

    for (int i = 0; i < nblocks; i++) {
        mult_blks[i] = math::multiply_const_cc::make_cpu({ k, veclen });
    }

    auto fg = flowgraph::make();

    fg->connect(src, 0, mult_blks[0], 0);
    for (int i = 1; i < nblocks; i++) {
        fg->connect(mult_blks[i - 1], 0, mult_blks[i], 0)
            ->set_custom_buffer(SM_BUFFER_ARGS);
    }
    fg->connect(mult_blks[nblocks - 1], 0, snk, 0);

    auto rt = runtime::make();
    rt->initialize(fg);
    rt->start();
    rt->wait();


    for (int i = 0; i < nsamples; i++) {
        expected_data[i] = gr_complex(k * 2 * i, k * (2 * i + 1));
    }


    auto d = snk->data();
    EXPECT_EQ(d, expected_data);
    EXPECT_EQ(d.size(), expected_data.size());

    for (size_t i = 0; i < expected_data.size(); i++) {
        if (d[i] != expected_data[i]) {
            std::cout << i << ": " << d[i] << " " << expected_data[i] << std::endl;
        }
    }
}


#if 1
// Test the case where we have multiple readers to a single block
TEST(SchedulerMTSingleBuffers, SingleMappedFanout)
{
    int nsamples = 1000000;
    std::vector<gr_complex> input_data(nsamples);
    std::vector<gr_complex> expected_data(nsamples);

    float k = 1.0;
    for (int i = 0; i < nsamples; i++) {
        input_data[i] = gr_complex(2 * i, 2 * i + 1);
        expected_data[i] = gr_complex(k * 2 * i, k * (2 * i + 1));
    }

    auto nblocks = 4;


    size_t veclen = 1;
    auto src = blocks::vector_source_c::make({ input_data });
    std::vector<blocks::vector_sink_c::sptr> sink_blks(nblocks);
    std::vector<math::multiply_const_cc::sptr> mult_blks(nblocks);

    for (int i = 0; i < nblocks; i++) {
        mult_blks[i] = math::multiply_const_cc::make_cpu({ k, veclen });
        sink_blks[i] = blocks::vector_sink_c::make({});
    }
    flowgraph_sptr fg(new flowgraph());

    for (int i = 0; i < nblocks; i++) {
        fg->connect(src, 0, mult_blks[i], 0)->set_custom_buffer(SM_BUFFER_ARGS);
        fg->connect(mult_blks[i], 0, sink_blks[i], 0)->set_custom_buffer(SM_BUFFER_ARGS);
    }

    auto rt = runtime::make();
    rt->initialize(fg);
    rt->start();
    rt->wait();

    for (int n = 0; n < nblocks; n++) {

        auto d = sink_blks[n]->data();
        EXPECT_EQ(d.size(), expected_data.size());
        if (d.size() == expected_data.size()) {
            EXPECT_EQ(d, expected_data);

            for (size_t i = 0; i < expected_data.size(); i++) {
                if (d[i] != expected_data[i]) {
                    std::cout << n << " " << i << ": " << d[i] << " " << expected_data[i]
                              << std::endl;
                }
            }
        }
        else {
            if (d.size() < expected_data.size()) {
                auto e = std::vector<gr_complex>(expected_data.begin(),
                                                 expected_data.begin() + d.size());
                EXPECT_EQ(d, e);

                for (size_t i = 0; i < e.size(); i++) {
                    if (d[i] != e[i]) {
                        std::cout << n << " " << i << ": " << d[i] << " " << e[i]
                                  << std::endl;
                    }
                }
            }
        }
    }
}
#endif

#include <gtest/gtest.h>

#include <iostream>
#include <thread>

#include <gnuradio/blocks/vector_sink.h>
#include <gnuradio/blocks/vector_source.h>
#include <gnuradio/flowgraph.h>
#include <gnuradio/math/multiply_const.h>
#include <gnuradio/runtime.h>
#include <gnuradio/schedulers/nbt/scheduler_nbt.h>

using namespace gr;

TEST(SchedulerBlockGrouping, BasicBlockGrouping)
{
    int nsamples = 1000000;
    std::vector<gr_complex> input_data(nsamples);
    std::vector<gr_complex> expected_data(nsamples);
    float k = 1.0;
    for (int i = 0; i < nsamples; i++) {
        input_data[i] = gr_complex(2 * i, 2 * i + 1);
        // expected_output[i] = gr_complex(k*2*i,k*2*i+1);
    }

    for (auto ngroups : { 2, 4 }) {
        for (auto nblocks : { 2, 4 }) {
            // for (auto nblocks : { 2, }) {
            size_t veclen = 1;
            auto src = blocks::vector_source_c::make_cpu(
                blocks::vector_source_c::block_args{ input_data, false });
            auto snk = blocks::vector_sink_c::make({});
            std::vector<math::multiply_const_cc::sptr> mult_blks(nblocks * ngroups);

            for (int i = 0; i < nblocks * ngroups; i++) {
                mult_blks[i] = math::multiply_const_cc::make({ k, veclen });
            }

            flowgraph_sptr fg(new flowgraph());

            auto sch = schedulers::scheduler_nbt::make("nbtsched");
            fg->connect(src, 0, mult_blks[0], 0);
            for (int n = 0; n < ngroups; n++) {
                std::vector<block_sptr> bg;
                for (int i = 0; i < nblocks; i++) {
                    int idx = n * nblocks + i;
                    if (idx > 0) {
                        fg->connect(mult_blks[idx - 1], 0, mult_blks[idx], 0);
                    }
                    bg.push_back(mult_blks[idx]);
                }
                sch->add_block_group(bg);
            }
            fg->connect(mult_blks[nblocks * ngroups - 1], 0, snk, 0);

            auto rt = runtime::make();
            rt->add_scheduler(sch);
            rt->initialize(fg);
            rt->start();
            rt->wait();

            std::cout << "ngroups: " << ngroups << ", nblocks: " << nblocks << std::endl;

            EXPECT_EQ(snk->data().size(), input_data.size());
            EXPECT_EQ(snk->data(), input_data);
        }
    }
}

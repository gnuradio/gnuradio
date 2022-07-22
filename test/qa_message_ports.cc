#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

#include <gnuradio/blocks/msg_forward.h>
#include <gnuradio/flowgraph.h>
#include <gnuradio/runtime.h>
#include <gnuradio/schedulers/nbt/scheduler_nbt.h>
#include <pmtf/string.hpp>

using namespace gr;

TEST(SchedulerMTMessagePassing, Forward)
{
    std::vector<float> input_data{ 1.0, 2.0, 3.0, 4.0, 5.0 };

    auto blk1 = blocks::msg_forward::make({});
    auto blk2 = blocks::msg_forward::make({});
    auto blk3 = blocks::msg_forward::make({});

    flowgraph_sptr fg(new flowgraph());
    fg->connect(blk1, "out", blk2, "in");
    fg->connect(blk2, "out", blk3, "in");

    auto rt = runtime::make();
    rt->initialize(fg);

    auto src_port = blk1->get_message_port("in");
    for (int i = 0; i < 10; i++) {
        src_port->post(pmtf::string("message"));
    }


    rt->start();

    // auto start = std::chrono::steady_clock::now();


    size_t cnt = 0;
    int num_iters = 0;
    while (true) {
        cnt = blk3->message_count();
        // auto end = std::chrono::steady_clock::now();
        if (cnt >= 10) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        num_iters++;
        if (num_iters >= 5) {
            break;
        }
    }

    EXPECT_EQ(cnt, 10);
    rt->stop();
}

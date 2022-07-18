#include <chrono>
#include <iostream>
#include <thread>

#include <gnuradio/blocks/msg_forward.h>
#include <gnuradio/flowgraph.h>
#include <gnuradio/realtime.h>
#include <gnuradio/runtime.h>
#include <pmtf/base.hpp>

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"

using namespace gr;

int main(int argc, char* argv[])
{
    uint64_t samples = 10000;
    uint64_t pdu_size = 100;
    unsigned int nblocks = 40;
    bool rt_prio = false;

    std::vector<unsigned int> cpu_affinity;

    CLI::App app{ "App description" };

    // app.add_option("-h,--help", "display help");
    app.add_option("--samples", samples, "Number of Bursts");
    app.add_option("--pdu_size", pdu_size, "PDU Size");
    app.add_option("--nblocks", nblocks, "Number of copy blocks");
    app.add_flag("--rt_prio", rt_prio, "Enable Real-time priority");
    app.add_option("--cpus",
                   cpu_affinity,
                   "Pin threads to CPUs (if nthreads > 0, will pin to 0,1,..,N");

    CLI11_PARSE(app, argc, argv);

    if (rt_prio && gr::enable_realtime_scheduling() != RT_OK) {
        std::cout << "Error: failed to enable real-time scheduling." << std::endl;
    }

    {
        std::vector<blocks::msg_forward::sptr> msg_blks(nblocks);
        for (size_t i = 0; i < nblocks; i++) {
            if (i == nblocks - 1)
                msg_blks[i] = blocks::msg_forward::make({ samples });
            else
                msg_blks[i] = blocks::msg_forward::make({ 0 });
        }

        flowgraph_sptr fg(new flowgraph());

        for (size_t i = 1; i < nblocks; i++) {
            fg->connect(msg_blks[i - 1], "out", msg_blks[i], "in");
        }

        auto rt = runtime::make();
        rt->initialize(fg);

        for (size_t p = 0; p < samples; p++) {
            pmtf::pmt msg = pmtf::vector<uint8_t>(pdu_size, 0x42);
            msg_blks[0]->input_message_port("in")->post(msg);
        }
        // msg_blks[0]->input_message_port("system")->post("done");

        // std::this_thread::sleep_for(std::chrono::seconds(3));
        auto t1 = std::chrono::steady_clock::now();


        rt->start();
        rt->wait();


        auto t2 = std::chrono::steady_clock::now();
        auto time =
            std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() / 1e9;

        std::cout << "[PROFILE_TIME]" << time << "[PROFILE_TIME]" << std::endl;

        // for (auto& b : msg_blks)
        // {
        //     std::cout << b->message_count() << ", ";
        // }
        // std::cout << std::endl;
    }
}

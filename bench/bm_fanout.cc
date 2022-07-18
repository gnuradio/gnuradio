#include <chrono>
#include <iostream>
#include <thread>

#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/buffer_cpu_simple.h>
#include <gnuradio/buffer_cpu_vmcirc.h>
#include <gnuradio/flowgraph.h>
#include <gnuradio/realtime.h>
#include <gnuradio/runtime.h>
#include <gnuradio/schedulers/nbt/scheduler_nbt.h>
#include <gnuradio/streamops/copy.h>
#include <gnuradio/streamops/head.h>

#include <iostream>

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"

using namespace gr;

int main(int argc, char* argv[])
{
    uint64_t samples = 15000000;
    int nblocks = 1;
    int veclen = 1;
    int buffer_type = 0;
    bool rt_prio = false;

    CLI::App app{ "App description" };

    app.add_option("--samples", samples, "Number of Samples");
    app.add_option("--veclen", veclen, "Vector Length");
    app.add_option("--nblocks", nblocks, "Number of copy blocks");
    app.add_option("--buffer_type",
                   buffer_type,
                   "Buffer Type (0:simple, 1:vmcirc, 2:cuda, 3:cuda_pinned");
    app.add_flag("--rt_prio", rt_prio, "Enable Real-time priority");

    if (rt_prio && gr::enable_realtime_scheduling() != RT_OK) {
        std::cout << "Error: failed to enable real-time scheduling." << std::endl;
    }

    {
        auto src = blocks::null_source::make({ sizeof(gr_complex) * veclen });
        auto head =
            streamops::head::make_cpu({ sizeof(gr_complex) * veclen, samples / veclen });

        std::vector<blocks::null_sink::sptr> sink_blks(nblocks);
        std::vector<streamops::copy::sptr> copy_blks(nblocks);
        for (int i = 0; i < nblocks; i++) {
            copy_blks[i] = streamops::copy::make({ sizeof(gr_complex) * veclen });
            sink_blks[i] = blocks::null_sink::make({ sizeof(gr_complex) * veclen });
        }
        flowgraph_sptr fg(new flowgraph());

        if (buffer_type == 0) {
            fg->connect(src, 0, head, 0);

            for (int i = 0; i < nblocks; i++) {
                fg->connect(head, 0, copy_blks[i], 0);
                fg->connect(copy_blks[i], 0, sink_blks[i], 0);
            }
        }
        else {
            fg->connect(src, 0, head, 0)->set_custom_buffer(BUFFER_CPU_VMCIRC_ARGS);

            for (int i = 0; i < nblocks; i++) {
                fg->connect(head, 0, copy_blks[i], 0)
                    ->set_custom_buffer(BUFFER_CPU_VMCIRC_ARGS);
                fg->connect(copy_blks[i], 0, sink_blks[i], 0)
                    ->set_custom_buffer(BUFFER_CPU_VMCIRC_ARGS);
            }
        }

        auto rt = runtime::make();
        rt->initialize(fg);

        auto t1 = std::chrono::steady_clock::now();

        rt->start();
        rt->wait();

        auto t2 = std::chrono::steady_clock::now();
        auto time =
            std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() / 1e9;

        std::cout << "[PROFILE_TIME]" << time << "[PROFILE_TIME]" << std::endl;
    }
}

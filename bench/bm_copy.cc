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

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"

using namespace gr;

int main(int argc, char* argv[])
{
    uint64_t samples = 15000000;
    unsigned int nblocks = 4;
    unsigned int nthreads = 0;
    int veclen = 1;
    int buffer_type = 1;
    int buffer_size = 32768;
    bool rt_prio = false;

    std::vector<unsigned int> cpu_affinity;

    CLI::App app{ "App description" };

    // app.add_option("-h,--help", "display help");
    app.add_option("--samples", samples, "Number of Samples");
    app.add_option("--veclen", veclen, "Vector Length");
    app.add_option("--nblocks", nblocks, "Number of copy blocks");
    app.add_option("--nthreads", nthreads, "Number of threads (0: tpb)");
    app.add_option("--buffer_type",
                   buffer_type,
                   "Buffer Type (0:simple, 1:vmcirc, 2:cuda, 3:cuda_pinned");
    app.add_option("--buffer_size", buffer_size, "Buffer Size in bytes");
    app.add_flag("--rt_prio", rt_prio, "Enable Real-time priority");
    app.add_option("--cpus",
                   cpu_affinity,
                   "Pin threads to CPUs (if nthreads > 0, will pin to 0,1,..,N");

    CLI11_PARSE(app, argc, argv);

    if (rt_prio && gr::enable_realtime_scheduling() != rt_status_t::OK) {
        std::cout << "Error: failed to enable real-time scheduling." << std::endl;
    }

    {
        auto src = blocks::null_source::make({ 1, sizeof(gr_complex) * veclen });
        auto head =
            streamops::head::make_cpu({ samples / veclen, sizeof(gr_complex) * veclen });
        auto snk = blocks::null_sink::make({ 1, sizeof(gr_complex) * veclen });
        std::vector<streamops::copy::sptr> copy_blks(nblocks);
        for (unsigned int i = 0; i < nblocks; i++) {
            copy_blks[i] = streamops::copy::make({ sizeof(gr_complex) * veclen });
        }
        flowgraph_sptr fg(new flowgraph());

        if (buffer_type == 0) {
            fg->connect(src, 0, head, 0);
            fg->connect(head, 0, copy_blks[0], 0);
            for (unsigned int i = 0; i < nblocks - 1; i++) {
                fg->connect(copy_blks[i], 0, copy_blks[i + 1], 0);
            }
            fg->connect(copy_blks[nblocks - 1], 0, snk, 0);
        }
        else {
            fg->connect(src, 0, head, 0)->set_custom_buffer(BUFFER_CPU_VMCIRC_ARGS);
            fg->connect(head, 0, copy_blks[0], 0)
                ->set_custom_buffer(BUFFER_CPU_VMCIRC_ARGS);
            for (unsigned int i = 0; i < nblocks - 1; i++) {
                fg->connect(copy_blks[i], 0, copy_blks[i + 1], 0)
                    ->set_custom_buffer(BUFFER_CPU_VMCIRC_ARGS);
            }
            fg->connect(copy_blks[nblocks - 1], 0, snk, 0)
                ->set_custom_buffer(BUFFER_CPU_VMCIRC_ARGS);
        }

        std::cout << "Initializing NBT scheduler with buffer size of " << buffer_size
                  << std::endl;
        auto sched = schedulers::scheduler_nbt::make("nbt", buffer_size);

        if (buffer_type == 1) {
            sched->set_default_buffer_factory(BUFFER_CPU_VMCIRC_ARGS);
        }

        if (nthreads > 0) {
            int blks_per_thread = nblocks / nthreads;

            for (unsigned int i = 0; i < nthreads; i++) {
                std::vector<block_sptr> block_group;
                if (i == 0) {
                    block_group.push_back(src);
                    block_group.push_back(head);
                }

                for (int j = 0; j < blks_per_thread; j++) {
                    block_group.push_back(copy_blks[i * blks_per_thread + j]);
                }

                if (i == nthreads - 1) {
                    for (unsigned int j = 0; j < (nblocks - nthreads * blks_per_thread);
                         j++) {
                        block_group.push_back(copy_blks[(i + 1) * blks_per_thread + j]);
                    }
                    block_group.push_back(snk);
                }
                if (cpu_affinity.empty()) {
                    sched->add_block_group(block_group);
                }
                else {
                    sched->add_block_group(
                        block_group, "group" + std::to_string(i), { cpu_affinity[i] });
                }
            }
        }

        auto rt = runtime::make();
        rt->add_scheduler(sched);
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

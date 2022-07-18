#include <chrono>
#include <iostream>
#include <thread>

#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/blocks/vector_sink.h>
#include <gnuradio/blocks/vector_source.h>
#include <gnuradio/flowgraph.h>
#include <gnuradio/logger.h>
#include <gnuradio/realtime.h>
#include <gnuradio/runtime.h>
#include <gnuradio/schedulers/nbt/scheduler_nbt.h>
#include <gnuradio/streamops/head.h>
#include <gnuradio/streamops/load.h>

#include <gnuradio/buffer_cpu_simple.h>
#include <gnuradio/buffer_cpu_vmcirc.h>
#include <gnuradio/buffer_cuda.h>
#include <gnuradio/buffer_cuda_pinned.h>
#include <gnuradio/buffer_cuda_sm.h>
#include <iostream>

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"

using namespace gr;

int main(int argc, char* argv[])
{
    uint64_t samples = 15000000;
    int mem_model = 0;
    size_t buffer_size = 8192;
    int nblocks = 4;
    size_t load = 1;
    bool rt_prio = false;
    bool single_mapped = false;
    bool use_cb = false;

    CLI::App app{ "App description" };

    // app.add_option("-h,--help", "display help");
    app.add_option("-N,--samples", samples, "Number of Samples");
    app.add_option("-l,--load", load, "Load");
    app.add_option("-b,--nblocks", nblocks, "Number of copy blocks");
    app.add_option("-m,--memmodel", mem_model, "Memory Model");
    app.add_option("-s,--bufsize", buffer_size, "Buffer Size in bytes");
    app.add_flag("--rt_prio", rt_prio, "Enable Real-time priority");
    app.add_flag("--sm", single_mapped, "Single Mapped");
    app.add_flag("--use_cb", use_cb, "Use Custom Buffers");

    CLI11_PARSE(app, argc, argv);

    if (rt_prio && gr::enable_realtime_scheduling() != RT_OK) {
        std::cout << "Error: failed to enable real-time scheduling." << std::endl;
    }

    std::vector<streamops::load::sptr> copy_blks(nblocks);
    for (int i = 0; i < nblocks; i++) {
        copy_blks[i] = streamops::load::make_cuda({ load, use_cb, sizeof(gr_complex) });
    }

    // std::vector<gr_complex> input_data(samples);
    // for (unsigned i = 0; i < samples; i++)
    //     input_data[i] = gr_complex(i % 256, 256 - i % 256);

    auto src = blocks::null_source::make({});
    auto snk = blocks::null_sink::make({});
    auto head = streamops::head::make_cpu({ samples });

    auto fg = flowgraph::make();

    fg->connect(src, 0, head, 0)
        ->set_custom_buffer(
            buffer_cpu_vmcirc_properties::make(buffer_cpu_vmcirc_type::AUTO)
                ->set_buffer_size(buffer_size));

    if (mem_model == 0) {
        if (single_mapped) {
            auto e = fg->connect(head, 0, copy_blks[0], 0);
            if (use_cb) {
                e->set_custom_buffer(
                    CUDA_BUFFER_SM_ARGS_H2D->set_buffer_size(buffer_size));
            }
        }
        else {
            auto e = fg->connect(head, 0, copy_blks[0], 0);
            if (use_cb) {
                e->set_custom_buffer(CUDA_BUFFER_ARGS_H2D->set_buffer_size(buffer_size));
            }
        }
        for (int i = 0; i < nblocks - 1; i++) {
            if (single_mapped) {
                auto e = fg->connect(copy_blks[i], 0, copy_blks[i + 1], 0);
                if (use_cb) {
                    e->set_custom_buffer(
                        CUDA_BUFFER_SM_ARGS_D2D->set_buffer_size(buffer_size));
                }
            }
            else {
                auto e = fg->connect(copy_blks[i], 0, copy_blks[i + 1], 0);
                if (use_cb) {
                    e->set_custom_buffer(
                        CUDA_BUFFER_ARGS_D2D->set_buffer_size(buffer_size));
                }
            }
        }
        if (single_mapped) {
            auto e = fg->connect(copy_blks[nblocks - 1], 0, snk, 0);
            if (use_cb) {
                e->set_custom_buffer(
                    CUDA_BUFFER_SM_ARGS_D2H->set_buffer_size(buffer_size));
            }
        }
        else {
            auto e = fg->connect(copy_blks[nblocks - 1], 0, snk, 0);
            if (use_cb) {
                e->set_custom_buffer(CUDA_BUFFER_ARGS_D2H->set_buffer_size(buffer_size));
            }
        }
    }
    else {
        auto e = fg->connect(head, 0, copy_blks[0], 0);
        if (use_cb) {
            e->set_custom_buffer(CUDA_BUFFER_PINNED_ARGS->set_buffer_size(buffer_size));
        }
        for (int i = 0; i < nblocks - 1; i++) {
            auto e = fg->connect(copy_blks[i], 0, copy_blks[i + 1], 0);
            if (use_cb) {
                e->set_custom_buffer(
                    CUDA_BUFFER_PINNED_ARGS->set_buffer_size(buffer_size));
            }
        }
        e = fg->connect(copy_blks[nblocks - 1], 0, snk, 0);
        if (use_cb) {
            e->set_custom_buffer(CUDA_BUFFER_PINNED_ARGS->set_buffer_size(buffer_size));
        }
    }

    // auto rt = runtime::make();
    // // rt->add_scheduler(sched);
    // rt->initialize(fg);

    if (rt_prio && gr::enable_realtime_scheduling() != gr::rt_status_t::RT_OK)
        std::cout << "Unable to enable realtime scheduling " << std::endl;

    auto t1 = std::chrono::steady_clock::now();
    fg->start();
    fg->wait();

    auto t2 = std::chrono::steady_clock::now();
    auto time =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() / 1e9;

    std::cout << "[PROFILE_TIME]" << time << "[PROFILE_TIME]" << std::endl;
}

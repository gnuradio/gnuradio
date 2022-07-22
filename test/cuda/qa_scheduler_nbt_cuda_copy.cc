#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

#include <gnuradio/blocks/vector_sink.h>
#include <gnuradio/blocks/vector_source.h>
// #include <gnuradio/cuda/copy.h>
#include <gnuradio/buffer_cuda.h>
#include <gnuradio/buffer_cuda_pinned.h>
#include <gnuradio/buffer_cuda_sm.h>
#include <gnuradio/flowgraph.h>
#include <gnuradio/runtime.h>
#include <gnuradio/schedulers/nbt/scheduler_nbt.h>
#include <gnuradio/streamops/copy.h>

using namespace gr;

/*
 * Test a basic chain of copy blocks, all assigned to the same thread
 *
 */
TEST(SchedulerMTTest, CudaCopyBasic)
{
    size_t veclen = 1024;
    int num_samples = veclen * 100;
    std::vector<gr_complex> input_data(num_samples);

    for (int i = 0; i < num_samples; i++) {
        input_data[i] = gr_complex(i, -i);
    }

    auto src = blocks::vector_source_c::make_cpu({ input_data, false, veclen });
    auto snk1 = blocks::vector_sink_c::make_cpu({ veclen });
    auto copy1 = streamops::copy::make_cuda({ veclen * sizeof(gr_complex) });
    auto copy2 = streamops::copy::make_cuda({ veclen * sizeof(gr_complex) });

    auto fg = flowgraph::make();
    fg->connect(src, 0, copy1, 0)->set_custom_buffer(CUDA_BUFFER_ARGS_H2D);
    fg->connect(copy1, 0, copy2, 0)->set_custom_buffer(CUDA_BUFFER_ARGS_D2D);
    fg->connect(copy2, 0, snk1, 0)->set_custom_buffer(CUDA_BUFFER_ARGS_D2H);

    auto sched = schedulers::scheduler_nbt::make("sched", 32768);
    sched->add_block_group({ copy1, copy2 });

    auto rt = runtime::make();
    rt->add_scheduler(sched);
    rt->initialize(fg);
    rt->start();
    rt->wait();

    EXPECT_EQ(snk1->data().size(), input_data.size());
    EXPECT_EQ(snk1->data(), input_data);
}

/*
 * Test a basic chain of copy blocks, on different threads
 *
 */
TEST(SchedulerMTTest, CudaCopyMultiThreaded)
{
    size_t veclen = 1024;
    int num_samples = veclen * 1000;
    std::vector<gr_complex> input_data(num_samples);

    for (int i = 0; i < num_samples; i++) {
        input_data[i] = gr_complex(i, -i);
    }

    auto src = blocks::vector_source_c::make_cpu({ input_data, false, veclen });
    auto snk1 = blocks::vector_sink_c::make_cpu({ veclen });
    auto copy1 = streamops::copy::make_cuda({ veclen * sizeof(gr_complex) });
    auto copy2 = streamops::copy::make_cuda({ veclen * sizeof(gr_complex) });

    auto fg = flowgraph::make();
    fg->connect(src, 0, copy1, 0)->set_custom_buffer(CUDA_BUFFER_ARGS_H2D);
    fg->connect(copy1, 0, copy2, 0)->set_custom_buffer(CUDA_BUFFER_ARGS_D2D);
    fg->connect(copy2, 0, snk1, 0)->set_custom_buffer(CUDA_BUFFER_ARGS_D2H);
    // fg->connect(src, 0, copy1, 0)->set_custom_buffer(CUDA_BUFFER_PINNED_ARGS);
    // fg->connect(copy1, 0, copy2, 0)->set_custom_buffer(CUDA_BUFFER_PINNED_ARGS);
    // fg->connect(copy2, 0, snk1, 0)->set_custom_buffer(CUDA_BUFFER_PINNED_ARGS);

    auto rt = runtime::make();
    rt->initialize(fg);
    rt->start();
    rt->wait();


    auto outdata = snk1->data();

    EXPECT_EQ(outdata.size(), input_data.size());
    EXPECT_EQ(outdata, input_data);

    // for (int i=0; i<outdata.size(); i++)
    // {
    //     if (outdata[i] != input_data[i])
    //     std::cout << outdata[i] << " " << input_data[i] << std::endl;
    // }
}


TEST(SchedulerMTTest, CudaCopySingleMapped)
{
    size_t veclen = 1024;
    int num_samples = veclen * 100;
    std::vector<gr_complex> input_data(num_samples);

    for (int i = 0; i < num_samples; i++) {
        input_data[i] = gr_complex(i, -i);
    }

    auto src = blocks::vector_source_c::make_cpu({ input_data, false, veclen });
    auto snk1 = blocks::vector_sink_c::make_cpu({ veclen });
    auto copy1 = streamops::copy::make_cuda({ veclen * sizeof(gr_complex) });
    auto copy2 = streamops::copy::make_cuda({ veclen * sizeof(gr_complex) });

    auto fg = flowgraph::make();
    fg->connect(src, 0, copy1, 0)->set_custom_buffer(CUDA_BUFFER_SM_ARGS_H2D);
    fg->connect(copy1, 0, copy2, 0)->set_custom_buffer(CUDA_BUFFER_SM_ARGS_D2D);
    fg->connect(copy2, 0, snk1, 0)->set_custom_buffer(CUDA_BUFFER_SM_ARGS_D2H);

    auto sched = schedulers::scheduler_nbt::make("sched", 32768);
    sched->add_block_group({ copy1, copy2 });

    auto rt = runtime::make();
    rt->add_scheduler(sched);
    rt->initialize(fg);
    rt->start();
    rt->wait();

    EXPECT_EQ(snk1->data().size(), input_data.size());
    EXPECT_EQ(snk1->data(), input_data);
}

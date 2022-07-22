#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>

#include <gnuradio/blocks/vector_sink.h>
#include <gnuradio/blocks/vector_source.h>
#include <gnuradio/flowgraph.h>
#include <gnuradio/port.h>
#include <gnuradio/streamops/copy.h>

using namespace gr;

class myhier : public gr::hier_block
{
public:
    myhier() : hier_block("my_hier")
    {
        add_port(gr::port<float>::make("hin", gr::port_direction_t::INPUT));
        add_port(gr::port<float>::make("hout1", gr::port_direction_t::OUTPUT));
        add_port(gr::port<float>::make("hout2", gr::port_direction_t::OUTPUT));

        auto cp1 = gr::streamops::copy::make({ sizeof(float) });
        auto cp2 = gr::streamops::copy::make({ sizeof(float) });

        this->connect(base(), 0, cp1, 0);
        this->connect(base(), 0, cp2, 0);
        this->connect(cp1, 0, base(), 0);
        this->connect(cp2, 0, base(), 1);
    }
};

TEST(HierBlockTest, Basic)
{
    int nsamples = 100000;
    std::vector<float> input_data(nsamples);
    for (int i = 0; i < nsamples; i++) {
        input_data[i] = i;
    }
    auto src = blocks::vector_source_f::make({ input_data, false });
    auto hh = std::make_shared<myhier>();
    auto snk1 = blocks::vector_sink_f::make({});
    auto snk2 = blocks::vector_sink_f::make({});


    auto fg = flowgraph::make();
    fg->connect(src, 0, hh, 0);
    fg->connect(hh, 0, snk1, 0);
    fg->connect(hh, 1, snk2, 0);

    fg->start();
    fg->wait();

    EXPECT_EQ(snk1->data().size(), input_data.size());
    EXPECT_EQ(snk2->data().size(), input_data.size());
    EXPECT_EQ(snk1->data(), input_data);
    EXPECT_EQ(snk2->data(), input_data);
}

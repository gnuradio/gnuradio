#include <gtest/gtest.h>

#include <iostream>
#include <thread>

#include <gnuradio/registry.h>

#include <gnuradio/blocks/vector_sink.h>
#include <gnuradio/blocks/vector_source.h>
#include <gnuradio/buffer_cpu_vmcirc.h>
#include <gnuradio/flowgraph.h>
#include <gnuradio/math/multiply_const.h>
#include <gnuradio/schedulers/nbt/scheduler_nbt.h>
#include <gnuradio/streamops/copy.h>

using namespace gr;

static bool is_in_list(const std::vector<std::string>& list, const std::string_view name)
{
    auto it = std::find(std::begin(list), std::end(list), name);
    return (it != std::end(list));
}

TEST(Reflection, Basic)
{
    gr::registry::init();
    auto list = gr::registry::modules();
    EXPECT_TRUE(is_in_list(list, "math"));
    EXPECT_TRUE(is_in_list(list, "blocks"));

    list = gr::registry::blocks("math");
    EXPECT_TRUE(is_in_list(list, "multiply_const_ff"));
    EXPECT_TRUE(is_in_list(list, "multiply_const_cc"));

    list = gr::registry::blocks("blocks");
    EXPECT_TRUE(is_in_list(list, "vector_source_c"));
    EXPECT_TRUE(is_in_list(list, "vector_sink_f"));

    float orig_value = 12.0;
    std::map<std::string, pmtf::pmt> param_map{ { "k", orig_value }, { "vlen", 1 } };
    auto blk = gr::registry::factory("math", "multiply_const_ff")(param_map);
    EXPECT_EQ(blk->request_parameter_query("k"), orig_value);
    // list = gr::registry::parameters("blocks","multiply_const_ff");
    // Verify that "k" is on the list
    // Verify that "vlen" is on the list

    float newval = 17.3;
    blk->request_parameter_change("k", newval);
    EXPECT_EQ(blk->request_parameter_query("k"), newval);
}

/* -*- c++ -*- */
/*
 * Copyright 2026 Jimmy Fitzpatrick
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/blocks/sigmf_sink.h>
#include <gnuradio/blocks/vector_source.h>
#include <gnuradio/top_block.h>
#include <nlohmann/json.hpp>
#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <fstream>

BOOST_AUTO_TEST_CASE(sigmf_sink_run)
{
    // This test writes to the filesystem, so use a temporary directory.
    auto tmp_dir = std::filesystem::temp_directory_path() / "sigmf_test";
    std::filesystem::create_directories(tmp_dir);

    // Run a simple flowgraph which writes known samples to disk as a
    // SigMF recording.
    const std::string filename((tmp_dir / "foo").string());
    const std::string& datatype("cf32_le");
    const size_t num_elements(4);
    const gr_complex element_value(1, 0);

    gr::top_block_sptr tb = gr::make_top_block("top");

    std::vector<gr_complex> in(num_elements, element_value);
    gr::blocks::vector_source_c::sptr source = gr::blocks::vector_source_c::make(in);

    gr::blocks::sigmf_sink::sptr sink = gr::blocks::sigmf_sink::make(filename, datatype);
    tb->connect(source, 0, sink, 0);
    tb->run();

    // Check we can recover the input samples from the Dataset file.
    std::ifstream dataset_stream(filename + ".sigmf-data",
                                 std::ios::binary | std::ios::in);
    std::vector<gr_complex> buff(num_elements, gr_complex(0, 0));
    dataset_stream.read(reinterpret_cast<char*>(buff.data()),
                        num_elements * sizeof(gr_complex));
    BOOST_TEST(in == buff);

    // Check the Metadata file is compliant with the specification, and that we can
    // recover basic contents.
    std::ifstream meta_stream(filename + ".sigmf-meta", std::ios::in);
    nlohmann::json meta = nlohmann::json::parse(meta_stream);
    BOOST_TEST(meta.contains("global"));
    BOOST_TEST(meta.contains("captures"));
    BOOST_TEST(meta.contains("annotations"));
    BOOST_TEST(meta["captures"].is_array());
    BOOST_TEST(meta["annotations"].is_array());
    BOOST_TEST(meta["captures"][0]["core:sample_start"].get<uint64_t>() == 0);
    BOOST_TEST(meta["global"].contains("core:version"));
    BOOST_TEST(meta["global"]["core:datatype"].get<std::string>() == datatype);

    // Clean up the temporary directory so the test is idempotent.
    std::filesystem::remove_all(tmp_dir);
}
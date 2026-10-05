/* -*- c++ -*- */
/*
 * Copyright 2026 Jimmy Fitzpatrick
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_SIGMF_SINK_IMPL_H
#define INCLUDED_GR_SIGMF_SINK_IMPL_H

#include <gnuradio/blocks/sigmf_sink.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>

namespace gr {
namespace blocks {

class sigmf_sink_impl : public sigmf_sink
{
public:
    sigmf_sink_impl(const std::string& filename, const std::string& datatype);
    ~sigmf_sink_impl();
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
    bool stop() override;

private:
    /* Metadata file */
    std::filesystem::path d_metadata_path;
    std::ofstream d_metadata_stream;
    nlohmann::json
        d_metadata; // The specification requires a single top level JSON object.

    /* Dataset file */
    std::filesystem::path d_dataset_path;
    std::ofstream d_dataset_stream;

    size_t d_sizeof_stream_item;

    void open_metadata();
    void open_dataset();

    void write_metadata();
    void write_dataset(const char* in, const size_t num_bytes);

    void write_recording(const char* in,
                         const size_t num_bytes,
                         const bool update_metadata = false);

    template <typename T>
    void set_global_field(const std::string& name, const T& value);

    void add_captures_segment(size_t sample_start);
};

} /* namespace blocks */
} /* namespace gr */

#endif

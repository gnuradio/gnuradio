/* -*- c++ -*- */
/*
 * Copyright 2026 Jimmy Fitzpatrick
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sigmf_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <nlohmann/json.hpp>

namespace {

// Specification constants.
constexpr const char* SIGMF_VERSION = "1.2.6";
constexpr const char* SIGMF_META = ".sigmf-meta";
constexpr const char* SIGMF_DATA = ".sigmf-data";
constexpr const char* SIGMF_GLOBAL = "global";
constexpr const char* SIGMF_CAPTURES = "captures";
constexpr const char* SIGMF_ANNOTATIONS = "annotations";
constexpr const char* SIGMF_CORE_NAMESPACE = "core";

int get_sizeof_stream_item(const std::string& datatype)
{
    // Only support little-endian data types. Users can use the gr::blocks::endian_swap
    // block if they're on a big-endian system.
    if (datatype == "cf32_le") {
        return sizeof(float[2]);
    } else if (datatype == "rf32_le") {
        return sizeof(float);
    } else if (datatype == "ci16_le") {
        return sizeof(int16_t[2]);
    } else if (datatype == "ri16_le") {
        return sizeof(int16_t);
    } else if (datatype == "cu16_le") {
        return sizeof(uint16_t[2]);
    } else if (datatype == "ru16_le") {
        return sizeof(uint16_t);
    } else {
        throw std::invalid_argument("Unsupported datatype: " + datatype);
    }
}

inline std::string make_key(const std::string& ns, const std::string& name)
{
    return ns + ":" + name;
}
} /* namespace */

namespace gr {
namespace blocks {

sigmf_sink::sptr sigmf_sink::make(const std::string& filename,
                                  const std::string& datatype)
{
    return gnuradio::make_block_sptr<sigmf_sink_impl>(filename, datatype);
}

sigmf_sink_impl::sigmf_sink_impl(const std::string& filename, const std::string& datatype)
    : sync_block("sigmf_sink",
                 gr::io_signature::make(1, 1, get_sizeof_stream_item(datatype)),
                 gr::io_signature::make(0, 0, 0)),
      d_metadata_path(filename),
      d_metadata_stream(0),
      d_metadata(),
      d_dataset_path(filename),
      d_dataset_stream(0),
      d_sizeof_stream_item(get_sizeof_stream_item(datatype))
{
    // The specification recommends that the base file names for the Recording's
    // Dataset and Metadata files are the same - they differ only by their extension.
    d_metadata_path.replace_extension(SIGMF_META);
    d_dataset_path.replace_extension(SIGMF_DATA);

    // Open the recording on construction.
    open_metadata();
    open_dataset();

    // Set global fields required by the specification.
    set_global_field<std::string>("datatype", datatype);
    set_global_field<std::string>("version", SIGMF_VERSION);

    // The top-level Object must contain the captures and annotations Objects.
    // So, initialise them with empty arrays.
    nlohmann::json empty_array = nlohmann::json::array();
    d_metadata[SIGMF_CAPTURES] = empty_array;
    d_metadata[SIGMF_ANNOTATIONS] = empty_array;

    // The specification recommends at least one captures segment is defined.
    add_captures_segment(0);

    // Write to the metadata file on construction.
    write_metadata();
}

sigmf_sink_impl::~sigmf_sink_impl(){
    // No-op.
};

void sigmf_sink_impl::open_metadata()
{
    if (d_metadata_stream.is_open()) {
        d_metadata_stream.close();
    }

    // The file contents are cleared.
    d_metadata_stream.open(d_metadata_path, std::ios::out | std::ios::trunc);

    if (!d_metadata_stream.good()) {
        throw std::runtime_error("Error opening " + d_metadata_path.string());
    }
}

void sigmf_sink_impl::open_dataset()
{
    if (d_dataset_stream.is_open()) {
        d_dataset_stream.close();
    };
    d_dataset_stream.open(d_dataset_path, std::ios::out | std::ios::binary);
    if (!d_dataset_stream.good()) {
        throw std::runtime_error("Error opening " + d_dataset_path.string());
    }
}


void sigmf_sink_impl::write_metadata()
{
    d_metadata_stream << d_metadata.dump(4);
    d_metadata_stream.flush();
    if (!d_metadata_stream.good()) {
        throw std::runtime_error("Error writing to " + d_metadata_path.string());
    }
}

void sigmf_sink_impl::write_dataset(const char* in, size_t num_bytes)
{
    d_dataset_stream.write(in, num_bytes);
    if (!d_dataset_stream.good()) {
        throw std::runtime_error("Error writing to " + d_dataset_path.string());
    }
}

void sigmf_sink_impl::write_recording(const char* in,
                                      size_t num_bytes,
                                      bool update_metadata)
{
    if (update_metadata) {
        // Re-open the metadata, so it's contents get reset.
        open_metadata();
        write_metadata();
    }
    write_dataset(in, num_bytes);
}

// Global fields may take different value types.
template <typename T>
void sigmf_sink_impl::set_global_field(const std::string& name, const T& value)
{
    // We're trusting the caller to set a field in-line with the specification.
    const std::string key = make_key(SIGMF_CORE_NAMESPACE, name);
    d_metadata[SIGMF_GLOBAL][key] = value;
}

void sigmf_sink_impl::add_captures_segment(size_t sample_start)
{
    const std::string key = make_key(SIGMF_CORE_NAMESPACE, "sample_start");
    if (!d_metadata[SIGMF_CAPTURES].empty()) {
        auto& last = d_metadata[SIGMF_CAPTURES].back();
        if (last[key] >= sample_start) {
            throw std::runtime_error("Error adding captures segment, sample start "
                                     "must be strictly increasing");
        }
    }
    nlohmann::json seg;
    seg[key] = sample_start;
    d_metadata[SIGMF_CAPTURES].push_back(seg);
}

bool sigmf_sink_impl::stop()
{
    d_metadata_stream.flush();
    d_dataset_stream.flush();
    return true;
}

int sigmf_sink_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    const char* in = reinterpret_cast<const char*>(input_items[0]);
    write_recording(in, d_sizeof_stream_item * noutput_items);
    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */

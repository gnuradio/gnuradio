/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 * Copyright 2025 Marcus Müller <mmueller@gnuradio.org>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pmt/pmt.h"
#include "gnuradio/block.h"
#include <gnuradio/pmt_fmt.h>
#include <algorithm>

#include <gnuradio/tagged_stream_block.h>

namespace gr {

tagged_stream_block::tagged_stream_block(const std::string& name,
                                         io_signature::sptr input_signature,
                                         io_signature::sptr output_signature,
                                         const std::string& length_tag_key)
    : block(name, input_signature, output_signature),
      d_length_tag_key(pmt::string_to_symbol(length_tag_key)),
      d_n_input_items_reqd(input_signature->min_streams(), 0),
      d_length_tag_key_str(length_tag_key)
{
    set_tag_propagation_policy(TPP_TSB);
}

// This is evil hackery: We trick the scheduler into creating the right number of input
// items
void tagged_stream_block::forecast(int noutput_items,
                                   gr_vector_int& ninput_items_required)
{
    unsigned ninputs = ninput_items_required.size();
    for (unsigned i = 0; i < ninputs; i++) {
        if (i < d_n_input_items_reqd.size() && d_n_input_items_reqd[i] != 0) {
            ninput_items_required[i] = d_n_input_items_reqd[i];
        } else {
            // If there's no item, there's no tag--so there must at least be one!
            ninput_items_required[i] = std::max(
                1, (int)std::floor((double)noutput_items / relative_rate() + 0.5));
        }
    }
}

void tagged_stream_block::get_tags_in_range(std::vector<tag_t>& v,
                                            unsigned int which_input,
                                            uint64_t abs_start,
                                            uint64_t abs_end)
{
    std::vector<tag_t> tmp;
    gr::block::get_tags_in_range(tmp, which_input, abs_start, abs_end);
    v.clear();
    for (const auto& tag : tmp) {
        if (!pmt::eqv(tag.key, d_length_tag_key)) {
            v.push_back(tag);
        }
    }
}
void tagged_stream_block::get_tags_in_range(std::vector<tag_t>& v,
                                            unsigned int which_input,
                                            uint64_t abs_start,
                                            uint64_t abs_end,
                                            const pmt::pmt_t& key)
{
    std::vector<tag_t> tmp;
    gr::block::get_tags_in_range(tmp, which_input, abs_start, abs_end, key);
    v.clear();
    for (const auto& tag : tmp) {
        if (!pmt::eqv(tag.key, d_length_tag_key)) {
            v.push_back(tag);
        }
    }
}

void tagged_stream_block::get_tags_in_window(std::vector<tag_t>& v,
                                             unsigned int which_input,
                                             uint64_t start,
                                             uint64_t end)
{
    std::vector<tag_t> tmp;
    gr::block::get_tags_in_window(tmp, which_input, start, end);
    v.clear();
    for (const auto& tag : tmp) {
        if (!pmt::eqv(tag.key, d_length_tag_key)) {
            v.push_back(tag);
        }
    }
}

void tagged_stream_block::get_tags_in_window(std::vector<tag_t>& v,
                                             unsigned int which_input,
                                             uint64_t start,
                                             uint64_t end,
                                             const pmt::pmt_t& key)
{
    std::vector<tag_t> tmp;
    gr::block::get_tags_in_window(tmp, which_input, start, end, key);
    v.clear();
    for (const auto& tag : tmp) {
        if (!pmt::eqv(tag.key, d_length_tag_key)) {
            v.push_back(tag);
        }
    }
}

void tagged_stream_block::parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
                                            gr_vector_int& n_input_items_reqd)
{
    for (unsigned input_idx = 0; input_idx < tags.size(); input_idx++) {
        for (const auto& tag : tags[input_idx]) {
            if (tag.key == d_length_tag_key) {
                n_input_items_reqd[input_idx] = pmt::to_long(tag.value);
            }
        }
    }
}

int tagged_stream_block::calculate_output_stream_length(const gr_vector_int& ninput_items)
{
    int noutput_items = *std::max_element(ninput_items.begin(), ninput_items.end());
    return (int)std::floor(relative_rate() * noutput_items + 0.5);
}

void tagged_stream_block::update_length_tags(int n_produced, int n_ports)
{
    for (int i = 0; i < n_ports; i++) {
        add_item_tag(i, nitems_written(i), d_length_tag_key, pmt::from_long(n_produced));
    }
    return;
}

bool tagged_stream_block::check_topology(int ninputs, int /* noutputs */)
{
    d_n_input_items_reqd.resize(ninputs, 0);
    return true;
}


int tagged_stream_block::general_work(int noutput_items,
                                      gr_vector_int& ninput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    if (d_length_tag_key_str.empty()) {
        return work(noutput_items, ninput_items, input_items, output_items);
    }

    std::vector<std::vector<tag_t>> tags(input_items.size(), std::vector<tag_t>());
    // Read TSB tags, unless we...
    // ...don't have inputs or ...     ... we already set it in a previous run.
    if (!d_n_input_items_reqd.empty() && d_n_input_items_reqd[0] == 0) {
        for (unsigned i = 0; i < input_items.size(); i++) {
            gr::block::get_tags_in_range(tags[i], i, nitems_read(i), nitems_read(i) + 1);
        }
        d_n_input_items_reqd.assign(input_items.size(), -1);
        parse_length_tags(tags, d_n_input_items_reqd);
    }
    for (unsigned i = 0; i < input_items.size(); i++) {
        if (d_n_input_items_reqd[i] == -1) {
            d_logger->fatal("Missing a required length tag on port {:d} at item #{:d}",
                            i,
                            nitems_read(i));
            throw std::runtime_error("Missing length tag.");
        }
        if (d_n_input_items_reqd[i] > ninput_items[i]) {
            return 0;
        }
    }

    int min_output_size = calculate_output_stream_length(d_n_input_items_reqd);
    if (noutput_items < min_output_size) {
        set_min_noutput_items(min_output_size);
        return 0;
    }
    set_min_noutput_items(1);

    // WORK CALLED HERE //
    int n_produced = work(noutput_items, d_n_input_items_reqd, input_items, output_items);
    //////////////////////

    if (n_produced == WORK_DONE) {
        return n_produced;
    }
    for (int i = 0; i < (int)d_n_input_items_reqd.size(); i++) {
        consume(i, d_n_input_items_reqd[i]);
    }
    if (n_produced > 0) {
        update_length_tags(n_produced, output_items.size());
        if (tag_propagation_policy() == TPP_TSB) {
            d_logger->trace("TPP_TSB policy: copying all non-length tags from all inputs "
                            "to all outputs");
            for (const auto& tag_vector : tags) {
                for (const auto& tag : tag_vector) {
                    if (!(pmt::eq(tag.key, d_length_tag_key))) {
                        for (size_t output = 0; output < output_items.size(); ++output) {
                            d_logger->trace("Copying tag {} to output {}", tag, output);
                            add_item_tag(output, tag);
                        }
                    }
                }
            }
        }
    }

    d_n_input_items_reqd.assign(input_items.size(), 0);

    return n_produced;
}

} /* namespace gr */

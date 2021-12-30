/* -*- c++ -*- */
/*
 * Copyright 2021 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: (GPL-3.0-or-later OR LGPL-3.0-or-later)
 *
 */

#pragma once
#include "gnuradio/io_signature.h"
#include "gnuradio/stream_iterators.h"
#include <gnuradio/api.h>
#include <gnuradio/block.h>
#include <string_view>

namespace gr {
struct GR_RUNTIME_API produced_t {
    size_t items_produced;
    bool done = false;
};
/*!
 * \brief synchronous 1:1 input to output with history
 * \ingroup base_blk
 *
 * Override work to provide the signal processing implementation.
 */
template <typename in_t = char, typename out_t = char>
class GR_RUNTIME_API typed_sync_block : public block
{
public:
    using in_type = in_t;
    using out_type = out_t;
    using inputs_t = stream_span<const in_type*>;
    using outputs_t = stream_span<out_type*>;

protected:
    typed_sync_block(void) {} // allows pure virtual interface sub-classes
    typed_sync_block(std::string_view name,
                     int min_in = 1,
                     int max_in = 1,
                     int min_out = 1,
                     int max_out = 1)
        : block(std::string(name),
                gr::io_signature::make(min_in, max_in, sizeof(in_t)),
                gr::io_signature::make(min_out, max_out, sizeof(out_t)))
    {
        set_fixed_rate(true);
    }
    /*!
     * \brief just like gr::block::general_work, only this arranges to
     * call consume_each for you
     *
     * The user must override work to define the signal processing code
     */
    virtual produced_t
    work(size_t n_items, inputs_t input_streams, outputs_t output_streams) = 0;

public:
    // gr::typed_sync_block overrides these to assist work
    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override
    {
        for (auto& ninput_required : ninput_items_required) {
            ninput_required = fixed_rate_noutput_to_ninput(noutput_items);
        }
    }

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override
    {
        auto retval =
            work(static_cast<size_t>(noutput_items), // number of output items
                 stream_span<const in_type*>         // input items
                 {
                     reinterpret_cast<const in_type**>(
                         input_items.data()), // pointer to beginning of input items
                     input_items.size(),      // number of input streams
                     static_cast<size_t>(noutput_items) // number of items
                 },
                 stream_span<out_type*> // output items
                 {
                     reinterpret_cast<out_type**>(
                         output_items.data()), // pointer to beginning of output items
                     output_items.size(),      // number of output streams
                     static_cast<size_t>(noutput_items) // number of items
                 });
        consume_each(retval.items_produced);
        if (retval.items_produced) {
            for (size_t output = 0; output < output_items.size(); ++output)
                produce(output, retval.items_produced);
        }
        return retval.done ? WORK_DONE : WORK_CALLED_PRODUCE;
    }

    int fixed_rate_ninput_to_noutput(int ninput) override
    {
        return std::max(0, ninput - static_cast<int>(history()) + 1);
    }

    int fixed_rate_noutput_to_ninput(int noutput) override
    {
        return noutput + static_cast<int>(history()) - 1;
    }
};
} // namespace gr

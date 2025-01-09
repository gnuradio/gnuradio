/* -*- c++ -*- */
/*
 * Copyright 2004,2008-2010,2013,2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/block.h>
#include <gnuradio/block_detail.h>
#include <gnuradio/custom_lock.h>
#include <gnuradio/prefs.h>
#include <block_executor.h>
#include <limits>
#include <sstream>


namespace gr {

// must be defined to either 0 or 1
#define ENABLE_LOGGING 0

#if (ENABLE_LOGGING)
#define LOG(x) \
    do {       \
        x;     \
    } while (0)
#else
#define LOG(x) \
    do {       \
        ;      \
    } while (0)
#endif

inline static unsigned int round_up(unsigned int n, unsigned int multiple)
{
    return ((n + multiple - 1) / multiple) * multiple;
}

inline static unsigned int round_down(unsigned int n, unsigned int multiple)
{
    return (n / multiple) * multiple;
}

//
// Return minimum available write space in all our downstream
// buffers or -1 if we're output blocked and the output we're
// blocked on is done.
//
static int min_available_space(block* m,
                               block_detail* d,
                               int output_multiple,
                               int min_noutput_items,
                               int& output_idx)
{
#if ENABLE_LOGGING
    gr::logger_ptr logger;
    gr::logger_ptr debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "min_available_space");
#endif

    int min_space = std::numeric_limits<int>::max();
    if (min_noutput_items == 0)
        min_noutput_items = 1;
    for (int i = output_idx; i < d->noutputs(); i++) {
        buffer_sptr out_buf = d->output(i);
        gr::thread::scoped_lock guard(*out_buf->mutex());
        int space_avail = out_buf->space_available();
        int avail_n = round_down(space_avail, output_multiple);
        // If not strictly output multiple size aligned, potentially use all
        // space available in the output buffer
        if (avail_n == 0 && space_avail < output_multiple && !m->output_multiple_set()) {
            avail_n = std::max(avail_n, space_avail);
        }
        int best_n = round_down(out_buf->bufsize() / 2, output_multiple);
        if (best_n < min_noutput_items)
            throw std::runtime_error("Buffer too small for min_noutput_items");
        int n = std::min(avail_n, best_n);
        if (n < min_noutput_items) { // We're blocked on output.
            LOG(std::ostringstream msg;
                msg << m << " **[i=" << i << "] output_multiple=" << output_multiple
                    << " min_noutput_items=" << min_noutput_items
                    << " avail_n=" << avail_n << " best_n=" << best_n << " n=" << n
                    << " min_space=" << min_space << " outbuf_done=" << out_buf->done();
                GR_LOG_INFO(debug_logger, msg.str()););

            if (out_buf->done()) { // Downstream is done, therefore we're done.
                return -1;
            }

            output_idx = i;
            return 0;
        }
        min_space = std::min(min_space, n);

        LOG(std::ostringstream msg;
            msg << m << " [i=" << i << "] output_multiple=" << output_multiple
                << " min_noutput_items=" << min_noutput_items << " avail_n=" << avail_n
                << " best_n=" << best_n << " n=" << n << " min_space=" << min_space;
            GR_LOG_INFO(debug_logger, msg.str()););
    }
    return min_space;
}

static bool propagate_tags(block::tag_propagation_policy_t policy,
                           block_detail* d,
                           const std::vector<uint64_t>& start_nitems_read,
                           double rrate,
                           mpq_class& mp_rrate,
                           bool use_fp_rrate,
                           std::vector<tag_t>& rtags,
                           long block_id)
{
    static const mpq_class one_half(1, 2);

    // Move tags downstream
    // if a sink, we don't need to move downstream
    if (d->sink_p()) {
        return true;
    }

    switch (policy) {
    case block::TPP_DONT:
    case block::TPP_CUSTOM:
    case block::TPP_TSB:
        /* Don't copy anything.
         TPP_TSB: tagged stream blocks: tag copying handled by gr::tagged_stream_block
         superclass */
        return true;
    case block::TPP_ALL_TO_ALL: {
        // every tag on every input propagates to everyone downstream
        std::vector<buffer_sptr> out_buf;

        for (int i = 0; i < d->ninputs(); i++) {
            d->get_tags_in_range(rtags, i, start_nitems_read[i], d->nitems_read(i));

            if (rtags.empty()) {
                continue;
            }

            if (out_buf.empty()) {
                out_buf.reserve(d->noutputs());
                for (int o = 0; o < d->noutputs(); o++)
                    out_buf.push_back(d->output(o));
            }

            std::vector<tag_t>::iterator t;
            if (rrate == 1.0) {
                for (t = rtags.begin(); t != rtags.end(); t++) {
                    for (int o = 0; o < d->noutputs(); o++)
                        out_buf[o]->add_item_tag(*t);
                }
            } else if (use_fp_rrate) {
                for (t = rtags.begin(); t != rtags.end(); t++) {
                    tag_t new_tag = *t;
                    new_tag.offset = std::llround((double)new_tag.offset * rrate);
                    for (int o = 0; o < d->noutputs(); o++)
                        out_buf[o]->add_item_tag(new_tag);
                }
            } else {
                mpz_class offset;
                for (t = rtags.begin(); t != rtags.end(); t++) {
                    tag_t new_tag = *t;
                    mpz_import(offset.get_mpz_t(),
                               1,
                               1,
                               sizeof(new_tag.offset),
                               0,
                               0,
                               &new_tag.offset);
                    offset = offset * mp_rrate + one_half;
                    new_tag.offset = offset.get_ui();
                    for (int o = 0; o < d->noutputs(); o++)
                        out_buf[o]->add_item_tag(new_tag);
                }
            }
        }
    } break;
    case block::TPP_ONE_TO_ONE:
        // tags from input i only go to output i
        // this requires d->ninputs() == d->noutputs; this is checked when this
        // type of tag-propagation system is selected in block_detail
        if (d->ninputs() == d->noutputs()) {
            buffer_sptr out_buf;

            for (int i = 0; i < d->ninputs(); i++) {
                d->get_tags_in_range(rtags, i, start_nitems_read[i], d->nitems_read(i));

                if (rtags.empty()) {
                    continue;
                }

                out_buf = d->output(i);

                std::vector<tag_t>::iterator t;
                if (rrate == 1.0) {
                    for (t = rtags.begin(); t != rtags.end(); t++) {
                        out_buf->add_item_tag(*t);
                    }
                } else if (use_fp_rrate) {
                    for (t = rtags.begin(); t != rtags.end(); t++) {
                        tag_t new_tag = *t;
                        new_tag.offset = std::llround((double)new_tag.offset * rrate);
                        out_buf->add_item_tag(new_tag);
                    }
                } else {
                    mpz_class offset;
                    for (t = rtags.begin(); t != rtags.end(); t++) {
                        tag_t new_tag = *t;
                        mpz_import(offset.get_mpz_t(),
                                   1,
                                   1,
                                   sizeof(new_tag.offset),
                                   0,
                                   0,
                                   &new_tag.offset);
                        offset = offset * mp_rrate + one_half;
                        new_tag.offset = offset.get_ui();
                        out_buf->add_item_tag(new_tag);
                    }
                }
            }
        } else {
            std::ostringstream msg;
            msg << "block_executor: propagation_policy 'ONE-TO-ONE'";
            msg << " requires ninputs == noutputs";
            GR_LOG_ERROR(d->d_logger, msg.str());
            return false;
        }
        break;
    default:
        return true;
    }
    return true;
}

block_executor::block_executor(block_sptr block, int max_noutput_items)
    : d_block(block), d_max_noutput_items(max_noutput_items)
{
    gr::configure_default_loggers(d_logger, d_debug_logger, "block_executor");

#ifdef GR_PERFORMANCE_COUNTERS
    prefs* prefs = prefs::singleton();
    d_use_pc = prefs->get_bool("PerfCounters", "on", false);
#endif /* GR_PERFORMANCE_COUNTERS */

    d_block->start(); // enable any drivers, etc.
}

block_executor::~block_executor()
{
    d_block->stop(); // stop any drivers, etc.
}

block_executor::state block_executor::run_one_iteration()
{
    int noutput_items;
    int max_items_avail;
    int max_noutput_items;
    int new_alignment = 0;
    int alignment_state = -1;
    int output_idx = 0;

    block* m = d_block.get();
    block_detail* d = m->detail().get();

    LOG(std::ostringstream msg; msg << m; GR_LOG_INFO(d_debug_logger, msg.str()););

    max_noutput_items = round_down(d_max_noutput_items, m->output_multiple());

    if (d->done()) {
        d_logger->error("unexpected done() in run_one_iteration");
        return DONE;
    }

    if (d->source_p()) {
        d_ninput_items_required.resize(0);
        d_ninput_items.resize(0);
        d_input_items.resize(0);
        d_input_done.resize(0);
        d_output_items.resize(d->noutputs());
        d_start_nitems_read.resize(0);

        // determine the minimum available output space
        output_idx = 0;
    blkd_out_try_again:
        noutput_items = min_available_space(
            m, d, m->output_multiple(), m->min_noutput_items(), output_idx);
        noutput_items = std::min(noutput_items, max_noutput_items);
        LOG(std::ostringstream msg; msg << "source:  noutput_items = " << noutput_items;
            GR_LOG_INFO(d_debug_logger, msg.str()););
        if (noutput_items == -1) // we're done
            goto were_done;

        if (noutput_items == 0) { // we're output blocked
            LOG(std::ostringstream msg; msg << m << " -- BLKD_OUT";
                GR_LOG_INFO(d_debug_logger, msg.str()););

            buffer_sptr out_buf = d->output(output_idx);

            if (out_buf->output_blkd_cb_ready(m->output_multiple())) {
                gr::custom_lock lock(std::ref(*out_buf->mutex()), out_buf);
                if (!out_buf->output_blocked_callback(m->output_multiple())) {
                    LOG(std::ostringstream msg;
                        msg << m << " -- BLKD_OUT -- ([1] callback FAILED)";
                        GR_LOG_INFO(d_debug_logger, msg.str()););
                    return BLKD_OUT;
                } else {
                    LOG(std::ostringstream msg;
                        msg << m << " -- BLKD_OUT -- ([1] try again idx: " << output_idx
                            << ")";
                        GR_LOG_INFO(d_debug_logger, msg.str()););
                    goto blkd_out_try_again;
                }
            } else {
                return BLKD_OUT;
            }
        }

        goto setup_call_to_work; // jump to common code
    }

    else if (d->sink_p()) {
        d_ninput_items_required.resize(d->ninputs());
        d_ninput_items.resize(d->ninputs());
        d_input_items.resize(d->ninputs());
        d_input_done.resize(d->ninputs());
        d_output_items.resize(0);
        d_start_nitems_read.resize(d->ninputs());
        LOG(std::ostringstream msg; msg << m << " -- sink";
            GR_LOG_INFO(d_debug_logger, msg.str()););

        max_items_avail = 0;
        for (int i = 0; i < d->ninputs(); i++) {
            {
                // Acquire the mutex and grab local copies of items_available and done.
                buffer_reader_sptr in_buf = d->input(i);
                gr::thread::scoped_lock guard(*in_buf->mutex());
                d_ninput_items[i] = in_buf->items_available();
                d_input_done[i] = in_buf->done();
            }

            LOG(std::ostringstream msg;
                msg << m << " -- d_ninput_items[" << i << "] = " << d_ninput_items[i];
                GR_LOG_INFO(d_debug_logger, msg.str()););
            LOG(std::ostringstream msg;
                msg << m << " -- d_input_done[" << i << "] = " << d_input_done[i];
                GR_LOG_INFO(d_debug_logger, msg.str()););

            if (d_ninput_items[i] < m->output_multiple() && d_input_done[i])
                goto were_done;

            max_items_avail = std::max(max_items_avail, d_ninput_items[i]);

            // Estimate if we are going to be blocked so we can call the input blocked
            // callback on the offending input
            int tmp_noutput_items = (int)(max_items_avail * m->relative_rate());
            tmp_noutput_items = round_down(tmp_noutput_items, m->output_multiple());
            tmp_noutput_items = std::min(tmp_noutput_items, max_noutput_items);
            if (tmp_noutput_items == 0) {
                // NOTE: normally input_blkd_cb_ready() and input_blocked_callback()
                // need "ninput_items_required" but it hasn't been calculated
                // yet, so instead guesstimate input required is one more than currently
                // available and see if that unblocks the input.
                buffer_reader_sptr in_buf = d->input(i);
                if (in_buf->input_blkd_cb_ready(d_ninput_items[i] + 1)) {
                    gr::custom_lock lock(std::ref(*in_buf->mutex()), in_buf->buffer());
                    if (in_buf->input_blocked_callback(d_ninput_items[i] + 1,
                                                       d_ninput_items[i])) {
                        LOG(std::ostringstream msg; msg << m << " -- BLKD_IN";
                            GR_LOG_INFO(d_logger, msg.str()));
                        return BLKD_IN;
                    }

                    // Recalculate after successfully executing the input blocked callback
                    d_ninput_items[i] = in_buf->items_available();
                    max_items_avail = std::max(max_items_avail, d_ninput_items[i]);
                }
            }
        }

        // take a swag at how much output we can sink
        noutput_items = (int)(max_items_avail * m->relative_rate());
        noutput_items = round_down(noutput_items, m->output_multiple());
        noutput_items = std::min(noutput_items, max_noutput_items);
        LOG(std::ostringstream msg;
            msg << m << " -- max_items_avail = " << max_items_avail;
            GR_LOG_INFO(d_debug_logger, msg.str()););
        LOG(std::ostringstream msg; msg << m << " -- noutput_items = " << noutput_items;
            GR_LOG_INFO(d_debug_logger, msg.str()););

        if (noutput_items == 0) { // we're blocked on input
            LOG(std::ostringstream msg; msg << m << " -- BLKD_IN";
                GR_LOG_INFO(d_debug_logger, msg.str()));
            return BLKD_IN;
        }

        goto try_again; // Jump to code shared with regular case.
    }

    else {
        // do the regular thing
        d_ninput_items_required.resize(d->ninputs());
        d_ninput_items.resize(d->ninputs());
        d_input_items.resize(d->ninputs());
        d_input_done.resize(d->ninputs());
        d_output_items.resize(d->noutputs());
        d_start_nitems_read.resize(d->ninputs());

    blkd_in_try_again:
        max_items_avail = 0;
        for (int i = 0; i < d->ninputs(); i++) {
            {
                // Acquire the mutex and grab local copies of items_available and done.
                buffer_reader_sptr in_buf = d->input(i);
                gr::thread::scoped_lock guard(*in_buf->mutex());
                d_ninput_items[i] = in_buf->items_available();
                d_input_done[i] = in_buf->done();
            }
            max_items_avail = std::max(max_items_avail, d_ninput_items[i]);
        }

        // determine the minimum available output space
        output_idx = 0;
    blkd_out_try_again2:
        noutput_items = min_available_space(
            m, d, m->output_multiple(), m->min_noutput_items(), output_idx);
        if (ENABLE_LOGGING) {
            std::ostringstream msg;
            msg << m << " -- regular ";
            msg << m->relative_rate_i() << ":" << m->relative_rate_d();
            msg << "  max_items_avail = " << max_items_avail;
            msg << "  noutput_items = " << noutput_items;
            GR_LOG_INFO(d_debug_logger, msg.str());
        }
        if (noutput_items == -1) // we're done
            goto were_done;

        if (noutput_items == 0) { // we're output blocked
            LOG(std::ostringstream msg; msg << m << " -- BLKD_OUT";
                GR_LOG_INFO(d_debug_logger, msg.str()));

            buffer_sptr out_buf = d->output(output_idx);

            if (out_buf->output_blkd_cb_ready(m->output_multiple())) {
                // Call the output blocked callback which will tell us if it was
                // able to unblock the output
                gr::custom_lock lock(std::ref(*out_buf->mutex()), out_buf);
                if (!out_buf->output_blocked_callback(m->output_multiple())) {
                    LOG(std::ostringstream msg;
                        msg << m << " -- BLKD_OUT -- ([2] callback FAILED)";
                        GR_LOG_INFO(d_debug_logger, msg.str()););
                    return BLKD_OUT;
                } else {
                    LOG(std::ostringstream msg;
                        msg << m << " -- BLKD_OUT -- ([2] try again idx: " << output_idx
                            << ")";
                        GR_LOG_INFO(d_debug_logger, msg.str()););
                    goto blkd_out_try_again2;
                }
            } else {
                return BLKD_OUT;
            }
        }

    try_again:
        if (m->fixed_rate()) {
            // Try to work it forward starting with max_items_avail.
            // We want to try to consume all the input we've got.
            int reqd_noutput_items = m->fixed_rate_ninput_to_noutput(max_items_avail);

            // only test this if we specifically set the output_multiple
            if (m->output_multiple_set())
                reqd_noutput_items = round_down(reqd_noutput_items, m->output_multiple());

            if (reqd_noutput_items > 0 && reqd_noutput_items <= noutput_items)
                noutput_items = reqd_noutput_items;

            // if we need this many outputs, overrule the max_noutput_items setting
            max_noutput_items = std::max(m->output_multiple(), max_noutput_items);
        }
        noutput_items = std::min(noutput_items, max_noutput_items);

        // Check if we're still unaligned; use up items until we're
        // aligned again. Otherwise, make sure we set the alignment
        // requirement.
        if (!m->output_multiple_set()) {
            if (m->is_unaligned()) {
                // When unaligned, don't just set noutput_items to the remaining
                // samples to meet alignment; this causes too much overhead in
                // requiring a premature call back here. Set the maximum amount
                // of samples to handle unalignment and get us back aligned.
                if (noutput_items >= m->unaligned()) {
                    noutput_items = round_up(noutput_items, m->alignment()) -
                                    (m->alignment() - m->unaligned());
                    new_alignment = 0;
                } else {
                    new_alignment = m->unaligned() - noutput_items;
                }
                alignment_state = 0;
            } else if (noutput_items < m->alignment()) {
                // if we don't have enough for an aligned call, keep track of
                // misalignment, set unaligned flag, and proceed.
                new_alignment = m->alignment() - noutput_items;
                m->set_unaligned(new_alignment);
                m->set_is_unaligned(true);
                alignment_state = 1;
            } else {
                // enough to round down to the nearest alignment and process.
                noutput_items = round_down(noutput_items, m->alignment());
                m->set_is_unaligned(false);
                alignment_state = 2;
            }
        }

        // ask the block how much input they need to produce noutput_items
        m->forecast(noutput_items, d_ninput_items_required);
        LOG(std::ostringstream msg;
            msg << m << " -- FCAST noutput_items=" << noutput_items << " inputs_required="
                << d_ninput_items_required[0] << " inputs_avail=" << d_ninput_items[0];
            GR_LOG_INFO(d_debug_logger, msg.str()));

        // See if we've got sufficient input available and make sure we
        // didn't overflow on the input.
        int i;
        for (i = 0; i < d->ninputs(); i++) {
            if (d_ninput_items_required[i] > d_ninput_items[i]) // not enough
                break;

            if (d_ninput_items_required[i] < 0) {
                std::ostringstream msg;
                msg << "sched: <block " << m->name() << " (" << m->unique_id() << ")>"
                    << " thinks its ninput_items required is "
                    << d_ninput_items_required[i] << " and cannot be negative."
                    << "Some parameterization is wrong. "
                    << "Too large a decimation value?";
                GR_LOG_ERROR(d_logger, msg.str());
                goto were_done;
            }
        }

        if (i < d->ninputs()) { // not enough input on input[i]
            // if we can, try reducing the size of our output request
            if (noutput_items > m->output_multiple()) {
                noutput_items /= 2;
                noutput_items = round_up(noutput_items, m->output_multiple());
                goto try_again;
            }

            // We're blocked on input
            LOG(std::ostringstream msg; msg << m << " -- BLKD_IN";
                GR_LOG_INFO(d_debug_logger, msg.str()));

            buffer_reader_sptr in_buf = d->input(i);

            if (in_buf->input_blkd_cb_ready(d_ninput_items_required[i])) {
                gr::custom_lock lock(std::ref(*in_buf->mutex()), in_buf->buffer());
                if (in_buf->input_blocked_callback(d_ninput_items_required[i],
                                                   d_ninput_items[i])) {
                    LOG(std::ostringstream msg; msg << m << " -- BLKD_IN -- TRY AGAIN";
                        GR_LOG_INFO(d_debug_logger, msg.str()));
                    goto blkd_in_try_again;
                }
            }

            if (d_input_done[i]) // If the upstream block is done, we're done
                goto were_done;

            // Is it possible to ever fulfill this request?
            if (d_ninput_items_required[i] > in_buf->max_possible_items_available()) {
                // Nope, never going to happen...
                std::ostringstream msg;
                msg << "sched: <block " << m->name() << " (" << m->unique_id() << ")>"
                    << " is requesting more input data"
                    << "  than we can provide."
                    << "  ninput_items_required = " << d_ninput_items_required[i]
                    << "  max_possible_items_available = "
                    << in_buf->max_possible_items_available()
                    << "  If this is a filter, consider reducing the number of taps.";
                GR_LOG_ERROR(d_logger, msg.str());
                goto were_done;
            }

            // If we were made unaligned in this round but return here without
            // processing; reset the unalignment claim before next entry.
            if (alignment_state == 1) {
                m->set_unaligned(0);
                m->set_is_unaligned(false);
            }
            return BLKD_IN;
        }

        // We've got enough data on each input to produce noutput_items.
        // Finish setting up the call to work.
        for (int i = 0; i < d->ninputs(); i++) {
            d->input(i)->buffer()->increment_active();
            d_input_items[i] = d->input(i)->read_pointer();
        }

    setup_call_to_work:

        d->d_produce_or = 0;
        for (int i = 0; i < d->noutputs(); i++) {
            d->output(i)->increment_active();
            d_output_items[i] = d->output(i)->write_pointer();
        }

        // determine where to start looking for new tags
        for (int i = 0; i < d->ninputs(); i++)
            d_start_nitems_read[i] = d->nitems_read(i);

#ifdef GR_PERFORMANCE_COUNTERS
        if (d_use_pc)
            d->start_perf_counters();
#endif /* GR_PERFORMANCE_COUNTERS */

        // Do the actual work of the block
        int n =
            m->general_work(noutput_items, d_ninput_items, d_input_items, d_output_items);

#ifdef GR_PERFORMANCE_COUNTERS
        if (d_use_pc)
            d->stop_perf_counters(noutput_items, n);
#endif /* GR_PERFORMANCE_COUNTERS */

        LOG(std::ostringstream msg;
            msg << m << " -- general_work: noutput_items = " << noutput_items
                << " ninput_items=" << (d->ninputs() >= 1 ? d_ninput_items[0] : 0)
                << " ninput_req=" << (d->ninputs() >= 1 ? d_ninput_items_required[0] : 0)
                << " result = " << n;
            GR_LOG_INFO(d_debug_logger, msg.str()););

        // Adjust number of unaligned items left to process
        if (m->is_unaligned()) {
            m->set_unaligned(new_alignment);
            m->set_is_unaligned(m->unaligned() != 0);
        }

        // Now propagate the tags based on the new relative rate
        if (!propagate_tags(m->tag_propagation_policy(),
                            d,
                            d_start_nitems_read,
                            m->relative_rate(),
                            m->mp_relative_rate(),
                            m->update_rate(),
                            d_returned_tags,
                            m->unique_id())) {
            d->post_work_cleanup();
            goto were_done;
        }

        if (n == block::WORK_DONE) {
            d->post_work_cleanup();
            goto were_done;
        }

        if (n != block::WORK_CALLED_PRODUCE) {
            d->produce_each(n); // advance write pointers
        }

        d->post_work_cleanup();

        // For some blocks that can change their produce/consume ratio
        // (the relative_rate), we might want to automatically update
        // based on the amount of items written/read.
        // In the block constructor, use enable_update_rate(true).
        if (m->update_rate()) {
            // rrate = ((double)(m->nitems_written(0))) / ((double)m->nitems_read(0));
            // if(rrate > 0.0)
            //  m->set_relative_rate(rrate);
            if ((n > 0) && (d->consumed() > 0))
                m->set_relative_rate((uint64_t)n, (uint64_t)d->consumed());
        }

        if (d->d_produce_or > 0) // block produced something
            return READY;

        // We didn't produce any output even though we called general_work.
        // We have (most likely) consumed some input.

        /*
        // If this is a source, it's broken.
        if(d->source_p()) {
          std::cerr << "block_executor: source " << m
                    << " produced no output.  We're marking it DONE.\n";
          // FIXME maybe we ought to raise an exception...
          goto were_done;
        }
        */

        // The call to general_work() produced no output therefore the block may
        // be "effectively output blocked". Call the output blocked callback
        // just in case, it can do no harm.
        for (int i = 0; i < d->noutputs(); i++) {
            buffer_sptr out_buf = d->output(i);
            LOG(std::ostringstream msg;
                msg << m << " -- NO OUTPUT -- [" << i << "] -- OUTPUT BLOCKED";
                GR_LOG_DEBUG(d_debug_logger, msg.str()););
            gr::custom_lock lock(std::ref(*out_buf->mutex()), out_buf);
            out_buf->output_blocked_callback(m->output_multiple(), true);
            LOG(std::ostringstream msg;
                msg << m << " -- NO OUTPUT -- [" << i << "] -- OUTPUT BLOCKED CBACK ";
                GR_LOG_DEBUG(d_debug_logger, msg.str()););
        }

        // Have the caller try again...
        return READY_NO_OUTPUT;
    }
    d_logger->error("invalid state while going through iteration state machine");

were_done:
    LOG(std::ostringstream msg; msg << m << " -- we're done";
        GR_LOG_INFO(d_debug_logger, msg.str()));
    d->set_done(true);
    return DONE;
}

} /* namespace gr */

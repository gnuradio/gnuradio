/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tags_to_pdu_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/pdu.h>
#include <chrono>

namespace gr {
namespace pdu {

template <class T>
typename tags_to_pdu<T>::sptr tags_to_pdu<T>::make(pmt::pmt_t start_tag,
                                                   pmt::pmt_t end_tag,
                                                   uint32_t max_pdu_size,
                                                   double samp_rate,
                                                   std::vector<T> prepend,
                                                   bool pub_start_msg,
                                                   uint32_t tail_size,
                                                   double start_time)
{
    return gnuradio::make_block_sptr<tags_to_pdu_impl<T>>(start_tag,
                                                          end_tag,
                                                          max_pdu_size,
                                                          samp_rate,
                                                          prepend,
                                                          pub_start_msg,
                                                          tail_size,
                                                          start_time);
}

/*
 * The private constructor
 */
template <class T>
tags_to_pdu_impl<T>::tags_to_pdu_impl(pmt::pmt_t start_tag,
                                      pmt::pmt_t end_tag,
                                      uint32_t max_pdu_size,
                                      double samp_rate,
                                      std::vector<T> prepend,
                                      bool pub_start_msg,
                                      uint32_t tail_size,
                                      double start_time)
    : gr::sync_block("tags_to_pdu",
                     gr::io_signature::make(1, 1, sizeof(T)),
                     gr::io_signature::make(0, 0, 0)),
      d_sob_tag_key(start_tag),
      d_eob_tag_key(end_tag),
      d_time_tag_key(metadata_keys::rx_time()),
      d_max_pdu_size(max_pdu_size),
      d_samp_rate(samp_rate),
      d_prepend(prepend),
      d_pub_start_msg(pub_start_msg),
      d_tail_size(tail_size),
      d_triggered(false),
      d_burst_counter(0),
      d_sob_tag_offset(0),
      d_meta_dict(pmt::make_dict()),
      d_wall_clock_time(false)
{
    // start times that will have roundoff issues are outside the intentions of this
    // parameter
    set_start_time(start_time);
    set_eob_parameters(1, 0);
    this->d_logger->notice(
        "starting at time {{{:d} {:f}}}", d_known_time_int_sec, d_known_time_frac_sec);

    this->d_logger->notice("rate {:.12f}", d_samp_rate);

    this->message_port_register_in(msgport_names::conf());
    this->set_msg_handler(msgport_names::conf(),
                          [this](pmt::pmt_t msg) { this->handle_ctrl_msg(msg); });
    this->message_port_register_out(msgport_names::pdus());
    this->message_port_register_out(msgport_names::detects());
}

/*
 * Our virtual destructor.
 */
template <class T>
tags_to_pdu_impl<T>::~tags_to_pdu_impl()
{
}

template <class T>
void tags_to_pdu_impl<T>::handle_ctrl_msg(pmt::pmt_t ctrl_msg)
{
    // if we do not receive a dictionary, don't do anything:
    if (!pmt::is_dict(ctrl_msg))
        return;

    // check dict for EOB offset command
    if (pmt::dict_has_key(ctrl_msg, eob_offset())) {
        uint32_t new_eob_offset = pmt::to_uint64(
            pmt::dict_ref(ctrl_msg, eob_offset(), pmt::from_uint64(d_eob_offset)));
        set_eob_parameters(d_eob_alignment, new_eob_offset);
        this->d_logger->notice("command received - set EOB tag offset to {:d} symbols",
                               d_eob_offset);
    }

    // check dict for EOB alignment command
    if (pmt::dict_has_key(ctrl_msg, eob_alignment())) {
        uint32_t new_eob_alignment = pmt::to_uint64(
            pmt::dict_ref(ctrl_msg, eob_alignment(), pmt::from_uint64(d_eob_alignment)));
        if (new_eob_alignment > 0) {
            set_eob_parameters(new_eob_alignment, d_eob_offset);
            this->d_logger->notice(
                "command received - set EOB tag alignment to {:d} symbols",
                d_eob_alignment);
        } else {
            this->d_logger->error("command received - illegal value {:d} for EOB "
                                  "alignment, not setting",
                                  new_eob_alignment);
        }
    }

    // handle other control messages...
}


template <class T>
void tags_to_pdu_impl<T>::publish_message()
{
    /* determine the time. we always have the offset that the SOB tag
     * was received on, we know the rate, and also have a known time/offset
     * pair - calculate the delta and apply accordingly.
     */
    double delta;
    delta = ((int64_t)d_sob_tag_offset - (int64_t)d_known_time_offset) / d_samp_rate;
    int int_delta = (int)delta;
    delta -= int_delta;

    uint64_t int_seconds = d_known_time_int_sec + int_delta;
    double frac_seconds = d_known_time_frac_sec + delta;

    // keep fractional seconds in range [0, 1.0)
    if (frac_seconds >= 1.0) {
        frac_seconds -= 1.0;
        int_seconds += 1;
    }
    pmt::pmt_t time_tuple =
        pmt::make_tuple(pmt::from_uint64(int_seconds), pmt::from_double(frac_seconds));
    d_meta_dict = pmt::dict_add(d_meta_dict, metadata_keys::rx_time(), time_tuple);
    d_meta_dict = pmt::dict_add(
        d_meta_dict, metadata_keys::sample_rate(), pmt::from_double(d_samp_rate));
    d_meta_dict = pmt::dict_add(
        d_meta_dict, metadata_keys::pdu_num(), pmt::from_uint64(d_burst_counter));
    if (d_wall_clock_time) {
        double t_now(std::chrono::duration<double>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count());
        d_meta_dict = pmt::dict_add(
            d_meta_dict, metadata_keys::sys_time(), pmt::from_double(t_now));
    }
    if (d_vector.size() > d_tail_size)
        this->message_port_pub(msgport_names::pdus(),
                               pmt::cons(d_meta_dict, init_data(d_vector)));

    // prepare for next burst
    d_burst_counter++;
    d_triggered = false;
    d_vector.clear();
}

template <class T>
int tags_to_pdu_impl<T>::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const T* in = (const T*)input_items[0];
    uint32_t consumed = noutput_items;

    uint64_t a_start = this->nitems_read(0);
    uint64_t a_end = a_start + noutput_items;

    // get all tags:
    this->get_tags_in_range(d_tags, 0, a_start, a_end);

    // find first SOB/EOB tag and process an time/offset tags encountered
    d_tag_type = NONE;
    for (size_t ii = 0; ii < d_tags.size(); ii++) {
        d_tag = d_tags[ii];
        if (pmt::eqv(d_tag.key, d_sob_tag_key)) {
            d_tag_type = SOB;
            consumed = d_tag.offset - a_start + 1;
            break;
        } else if (pmt::eqv(d_tag.key, d_eob_tag_key)) {
            d_tag_type = EOB;
            consumed = d_tag.offset - a_start + 1;
            break;
        } else if (pmt::eqv(d_tag.key, d_time_tag_key)) {
            set_known_time_offset(pmt::to_uint64(pmt::tuple_ref(d_tag.value, 0)),
                                  pmt::to_double(pmt::tuple_ref(d_tag.value, 1)),
                                  d_tag.offset);
        }
    }

    /* if the system is already triggered (has received SOB), we will be storing data
     * until we reach an EOB tag or the maximum PDU size is reached - regardless of what
     * happens we will consume data up to and including the received tag
     */
    if (d_triggered) {

        uint64_t last_addr_in_vec = (d_max_pdu_size - d_vector.size()) + a_start - 1;

        /* if we got an EOB/SOB tag, and the tag offset is before the max pdu
         * length, we need to take action on it
         */
        if ((d_tag_type != NONE) && (d_tag.offset <= last_addr_in_vec)) {

            if (d_tag_type == EOB) {

                // for EOB, always append data up to  (not including) tagged sample
                d_vector.insert(d_vector.end(), &in[0], &in[consumed - 1]);

                // check to see if the EOB tag is correctly aligned within the burst
                size_t n_aligned_needed =
                    (d_vector.size() - d_eob_offset) % d_eob_alignment;
                if (n_aligned_needed == 0) {
                    publish_message();

                } else {
                    // if misaligned, publish immediately and don't worry about it
                    for (size_t i = 0; i < (d_eob_alignment - n_aligned_needed); i++) {
                        d_vector.push_back(0);
                    }
                    publish_message();
                }

                // if we have received a second SOB tag, reset and dump previous data
            } else if (d_tag_type == SOB) {
                this->d_logger->error("SOB tag received during burst {:d} at offset "
                                      "{:d}, previous burst dropped ({:d} tags total)",
                                      d_burst_counter,
                                      d_tag.offset,
                                      d_tags.size());

                // prepare for next burst
                d_burst_counter++;
                d_triggered = false;
                d_vector.clear();

                // do not consume SOB item so it can be handled next work() call
                consumed--;
            }

            // otherwise, consume data and evaluate if we need to publish a message
        } else {
            // don't consume the sample with the SOB tag...
            consumed--;
            d_vector.insert(d_vector.end(), &in[0], &in[consumed]);
            if (d_vector.size() >= d_max_pdu_size) {
                d_vector.resize(d_max_pdu_size);
                publish_message();
            }
        }

        /* if we are not triggered, we are waiting for an SOB tag, until that is
         * reached, save no data or do anything other than consume samples up to
         * and including the observed tag other than warn if EOB tags are seen
         */
    } else {
        if (d_tag_type == SOB) {
            // the sob tag offset will be used to determine received time
            d_sob_tag_offset = d_tag.offset;

            // store the prepended data
            d_vector.insert(d_vector.end(), d_prepend.begin(), d_prepend.end());
            d_vector.push_back(in[consumed - 1]); // store and consume the tagged sample
            d_triggered = true;

            if (d_pub_start_msg) {
                this->message_port_pub(msgport_names::detects(),
                                       pmt::from_uint64(d_sob_tag_offset));
            }

        } else if (d_tag_type == EOB) {
            // receiving an EOB sequence while not triggered is just random chance. No
            // warning necessary... log at INFO level
            this->d_logger->info("received unexpected EOB at offset {:d}", d_tag.offset);

        } else {
            // do nothing
        }
    }

    return consumed;
}

template <class T>
void tags_to_pdu_impl<T>::set_start_tag(pmt::pmt_t tag)
{
    gr::thread::scoped_lock l(this->d_setlock);

    d_sob_tag_key = tag;
}


template <class T>
void tags_to_pdu_impl<T>::set_end_tag(pmt::pmt_t tag)
{
    gr::thread::scoped_lock l(this->d_setlock);

    d_eob_tag_key = tag;
}


template <class T>
void tags_to_pdu_impl<T>::set_time_tag_key(pmt::pmt_t tag)
{
    gr::thread::scoped_lock l(this->d_setlock);

    d_time_tag_key = tag;
}


template <class T>
void tags_to_pdu_impl<T>::set_start_time(double start_time)
{
    gr::thread::scoped_lock l(this->d_setlock);

    uint64_t sec = (uint64_t)start_time;
    double frac = (start_time - sec);
    set_known_time_offset(sec, frac, 0);
}


template <class T>
void tags_to_pdu_impl<T>::set_known_time_offset(uint64_t int_sec,
                                                double frac_sec,
                                                uint64_t offset)
{
    d_known_time_int_sec = int_sec;
    d_known_time_frac_sec = frac_sec;
    d_known_time_offset = offset;
}


template <class T>
void tags_to_pdu_impl<T>::set_eob_parameters(uint32_t alignment, uint32_t offset)
{
    gr::thread::scoped_lock l(this->d_setlock);

    d_eob_alignment = alignment;
    d_eob_offset = offset;
}


template <class T>
void tags_to_pdu_impl<T>::set_prepend(std::vector<T> prepend)
{
    gr::thread::scoped_lock l(this->d_setlock);

    d_prepend = prepend;
}


template <class T>
void tags_to_pdu_impl<T>::set_samp_rate(double rate)
{
    gr::thread::scoped_lock l(this->d_setlock);

    d_samp_rate = rate;
}


template <class T>
void tags_to_pdu_impl<T>::set_tail_size(uint32_t size)
{
    gr::thread::scoped_lock l(this->d_setlock);

    d_tail_size = size;
}


template <class T>
void tags_to_pdu_impl<T>::set_max_pdu_size(uint32_t size)
{
    gr::thread::scoped_lock l(this->d_setlock);

    d_max_pdu_size = size;
}


template <class T>
void tags_to_pdu_impl<T>::enable_time_debug(bool enable)
{
    if (enable) {
        d_wall_clock_time = true;
    } else {
        d_wall_clock_time = false;
    }
}

template <class T>
const pmt::pmt_t tags_to_pdu_impl<T>::eob_alignment()
{
    static const pmt::pmt_t val = pmt::mp("eob_alignment");
    return val;
}
template <class T>
const pmt::pmt_t tags_to_pdu_impl<T>::eob_offset()
{
    static const pmt::pmt_t val = pmt::mp("eob_offset");
    return val;
}

template class tags_to_pdu<unsigned char>;
template class tags_to_pdu<short>;
template class tags_to_pdu<int>;
template class tags_to_pdu<float>;
template class tags_to_pdu<gr_complex>;
} /* namespace pdu */
} /* namespace gr */

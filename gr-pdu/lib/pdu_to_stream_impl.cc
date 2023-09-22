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

#include "pdu_to_stream_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/pdu.h>

#include <chrono>
#include <thread>

namespace gr {
namespace pdu {

template <class T>
typename pdu_to_stream<T>::sptr
pdu_to_stream<T>::make(early_pdu_behavior_t early_pdu_behavior, uint32_t max_queue_size)
{
    return gnuradio::make_block_sptr<pdu_to_stream_impl<T>>(early_pdu_behavior,
                                                            max_queue_size);
}

/* BEHAVIOR OF SUCCESSIVE BURSTS

  -- APPEND MODE --
    If an untimed message is received with data still queued from previous
    messages, it will be appended to the end of the same burst.
    If a timed message is received it will be queued and transmitted with
    its own tx_sob and tx_eob tags. This will potentially cause late bursts
    to be ignored downstream at the USRP sink block.

  -- DROP MODE --
    If messages are received in rapid succession, any that arrive while
    there is still data queued from previous PDUs will be dropped.

  -- BALK MODE --
    Same behavior as drop mode except an error will also be emitted

    #TODO   add an option such that if a burst will be late, change it to
            untimed mode to prevent it from being dropped by downstream driver
*/

/*
 * The private constructor
 */
template <class T>
pdu_to_stream_impl<T>::pdu_to_stream_impl(early_pdu_behavior_t early_pdu_behavior,
                                          uint32_t max_queue_size)
    : gr::sync_block("pdu_to_stream",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(1, 1, sizeof(T))),
      d_itemsize(sizeof(T)),
      d_max_queue_size(max_queue_size),
      d_time_tag(pmt::PMT_NIL)
{
    d_data.clear();
    d_pdu_queue.clear();

    if (early_pdu_behavior == pdu::EARLY_BURST_APPEND) {
        d_drop_early_bursts = false;
        d_early_burst_err = false;
    } else if (early_pdu_behavior == pdu::EARLY_BURST_DROP) {
        d_drop_early_bursts = true;
        d_early_burst_err = false;
    } else if (early_pdu_behavior == pdu::EARLY_BURST_BALK) {
        d_drop_early_bursts = true;
        d_early_burst_err = true;
    } else {
        throw std::invalid_argument("Invalid early burst behavior mode " +
                                    std::to_string(early_pdu_behavior));
    }

    this->message_port_register_in(msgport_names::pdus());
    this->set_msg_handler(msgport_names::pdus(),
                          [this](pmt::pmt_t msg) { this->store_pdu(msg); });
}

/*
 * Our virtual destructor.
 */
template <class T>
pdu_to_stream_impl<T>::~pdu_to_stream_impl()
{
}


/*
 * function validates PDUs and stores them in a queue for further processing
 */
template <class T>
void pdu_to_stream_impl<T>::store_pdu(pmt::pmt_t pdu)
{
    // check and see if there is already data in the vector, drop if not in an append mode
    if (d_drop_early_bursts & (d_data.size() | d_pdu_queue.size())) {
        if (d_early_burst_err) {
            this->d_logger->error(
                "PDU received before previous burst finished writing - dropped");
        }
        return;
    }

    // make sure PDU data is formed properly
    if (!(pmt::is_pdu(pdu))) {
        this->d_logger->error("PMT is not a PDU, dropping");
        return;
    }

    pmt::pmt_t meta = pmt::car(pdu);
    pmt::pmt_t v_data = pmt::cdr(pdu);

    if (pmt::length(v_data) != 0) {
        size_t v_itemsize = pmt::uniform_vector_itemsize(v_data);
        if (v_itemsize != d_itemsize) {
            this->d_logger->error("PDU received has incorrect itemsize ({:d} != {:d})",
                                  v_itemsize,
                                  d_itemsize);
            return;
        }

        // pdu data is valid and nonzero length, queue it
        if (d_pdu_queue.size() < d_max_queue_size) {
            d_pdu_queue.push_back(pdu);
            d_drop_ctr = 0;
        } else {
            d_drop_ctr++;
            this->d_logger->warn("Queue full, PDU dropped ({:d} dropped so far)",
                                 d_drop_ctr);
        }
    } else {
        this->d_logger->warn("zero size PDU ignored");
    }

    return;
}


/*
 * this function will pop PDUs off the queue and store the data, returns
 * the number of data elements queued
 */
template <class T>
uint32_t pdu_to_stream_impl<T>::queue_data()
{
    uint32_t data_size = 0;
    // this should only get called when there is data in the queue, but check
    // anyway and return if it is empty
    if (d_pdu_queue.empty()) {
        return data_size;
    }

    pmt::pmt_t pdu = d_pdu_queue.front();
    d_pdu_queue.pop_front();

    // only validated PDUs allowed into queue, validation not required here
    pmt::pmt_t meta = pmt::car(pdu);
    pmt::pmt_t v_data = pmt::cdr(pdu);

    // retrieve the data
    size_t nbytes = 0;
    size_t nitems = 0;
    const T* d_in = static_cast<const T*>(pmt::uniform_vector_elements(v_data, nbytes));
    nitems = nbytes / sizeof(T);

    // resize vector once and push elements onto back
    d_data.reserve(d_data.size() + nitems);
    for (size_t i = 0; i < nitems; ++i) {
        d_data.push_back(d_in[i]);
    }
    data_size += (uint32_t)nitems;

    // check if there are future PDUs queued that are untimed and queue them also
    bool done = false;
    while (!done) {
        if (d_pdu_queue.empty()) {
            done = true;
            break;
        }
        pmt::pmt_t pdu2 = d_pdu_queue.front();
        pmt::pmt_t meta2 = pmt::car(pdu2);
        if (pmt::dict_has_key(meta2, metadata_keys::tx_time())) {
            // the next PDU is timed, so we will handle that on the next call
            done = true;
        } else {
            d_pdu_queue.pop_front();
            v_data = pmt::cdr(pdu2);

            // retrieve elements
            d_in = static_cast<const T*>(pmt::uniform_vector_elements(v_data, nbytes));
            nitems = nbytes / sizeof(T);

            // resize vector once and push elements onto back
            d_data.reserve(d_data.size() + nitems);
            for (size_t i = 0; i < nitems; ++i) {
                d_data.push_back(d_in[i]);
            }
            data_size += (uint32_t)nitems;
        }
    }

    d_tag_sob = true;

    // if the burst is timed, calculate the time tag
    pmt::pmt_t pmt_time = pmt::dict_ref(meta, metadata_keys::tx_time(), pmt::PMT_NIL);
    if (pmt::is_tuple(pmt_time) && pmt::length(pmt_time) >= 2 &&
        pmt::is_uint64(pmt::tuple_ref(pmt_time, 0)) &&
        pmt::is_real(pmt::tuple_ref(pmt_time, 1))) {
        // it's a good tuple...
        d_time_tag = pmt_time;
    }

    // possibly is a pair
    if (pmt::is_pair(pmt_time) && pmt::is_uint64(pmt::car(pmt_time)) &&
        pmt::is_real(pmt::cdr(pmt_time))) {
        // it's a good pair...
        d_time_tag = pmt::make_tuple(pmt::car(pmt_time), pmt::cdr(pmt_time));
    }

    return data_size;
}

template <class T>
int pdu_to_stream_impl<T>::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    T* out = (T*)output_items[0];

    uint32_t produced = 0;
    int data_remaining = d_data.size();

    // if there are no data in the queue, see if more PDUs are ready
    if (data_remaining == 0) {
        if (d_pdu_queue.empty()) {
            // if we have nothing to do, sleep for a short duration to prevent rapid
            // successive calls and then return zero items
            std::this_thread::sleep_for(std::chrono::microseconds(25));
            return 0;
        }

        // fetch another PDU of data and update the size of the data
        data_remaining = queue_data();
        if (data_remaining == 0) {
            std::this_thread::sleep_for(std::chrono::microseconds(25));
            return 0;
        }
    } /* end if data_remaining == 0 */

    // data_remaining is not zero so go ahead and update
    if (d_tag_sob) {
        this->add_item_tag(
            0, this->nitems_written(0), metadata_keys::tx_sob(), pmt::PMT_T);
        d_tag_sob = false;

        // if there is a time tag waiting, use it then reset the time tag
        if (!pmt::eqv(d_time_tag, pmt::PMT_NIL)) {
            this->add_item_tag(
                0, this->nitems_written(0), metadata_keys::tx_time(), d_time_tag);
            d_time_tag = pmt::PMT_NIL;
        }
    }

    // if everything remaining will fit, send it and tag EOB
    if (data_remaining <= noutput_items) {
        memcpy(out, &d_data[0], d_itemsize * data_remaining);
        // tag last item "tx_eob, True"
        this->add_item_tag(0,
                           this->nitems_written(0) + data_remaining - 1,
                           metadata_keys::tx_eob(),
                           pmt::PMT_T);
        d_data.clear();
        produced = data_remaining;

        // not everything will fit... send first noutput_items from PDU
    } else {
        if (noutput_items) {
            memcpy(out, &d_data[0], d_itemsize * noutput_items);
            d_data.erase(d_data.begin(), d_data.begin() + noutput_items);
            produced = noutput_items;
        }
    }

    // Tell runtime system how many output items we produced.
    return produced;
}

template class pdu_to_stream<unsigned char>;
template class pdu_to_stream<short>;
template class pdu_to_stream<int>;
template class pdu_to_stream<float>;
template class pdu_to_stream<gr_complex>;
} /* namespace pdu */
} /* namespace gr */

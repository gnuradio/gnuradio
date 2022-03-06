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

#include "take_skip_to_pdu_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/pdu.h>

namespace gr {
namespace pdu {

template <class T>
typename take_skip_to_pdu<T>::sptr take_skip_to_pdu<T>::make(uint32_t take, uint32_t skip)
{
    return gnuradio::make_block_sptr<take_skip_to_pdu_impl<T>>(take, skip);
}

/*
 * The private constructor
 */
template <class T>
take_skip_to_pdu_impl<T>::take_skip_to_pdu_impl(uint32_t take, uint32_t skip)
    : gr::sync_block("take_skip_to_pdu",
                     gr::io_signature::make(1, 1, sizeof(T)),
                     gr::io_signature::make(0, 0, 0)),
      d_take(take),
      d_skip(skip),
      d_next(take),
      d_triggered(true),
      d_burst_counter(0),
      d_prev_byte(0)
{
    if (d_take == 0) {
        this->d_logger->fatal("TAKE value too small, must be > 0");
        throw std::invalid_argument("TAKE value out of bounds");
    }

    d_vector.clear();
    d_meta_dict = pmt::make_dict();
    this->message_port_register_out(msgport_names::pdus());

    this->d_logger->info("Starting Take Skip PDU Generator!");
}


/*
 * Our virtual destructor.
 */
template <class T>
take_skip_to_pdu_impl<T>::~take_skip_to_pdu_impl()
{
}

template <class T>
bool take_skip_to_pdu_impl<T>::stop()
{
    this->d_logger->notice("Generated {:d} PDUs", d_burst_counter);
    return true;
}

template <class T>
void take_skip_to_pdu_impl<T>::publish_message()
{
    d_meta_dict = pmt::dict_add(
        d_meta_dict, metadata_keys::pdu_num(), pmt::from_uint64(d_burst_counter));

    // publish message
    this->message_port_pub(msgport_names::pdus(),
                           pmt::cons(d_meta_dict, this->init_data(d_vector)));

    // prepare for next burst
    d_burst_counter++;
    d_triggered = false;
    d_vector.clear();
}


template <class T>
int take_skip_to_pdu_impl<T>::work(int noutput_items,
                                   gr_vector_const_void_star& input_items,
                                   gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(this->d_setlock);

    const T* in = (const T*)input_items[0];
    uint64_t consumed = noutput_items;
    bool start_stop = false;

    // figure out where the next start/stop point is
    if (d_next <= (this->nitems_read(0) + noutput_items)) {
        // we are going to start/stop during this buffer
        consumed = d_next - this->nitems_read(0);
        start_stop = true;
        if (d_triggered) {
            d_next += d_skip;
        } else {
            d_next += d_take;
        }
    }

    if (d_triggered) {
        // we are saving data
        // std::cout << "    adding  " << consumed << " items to the queue" << std::endl;
        d_vector.insert(d_vector.end(), &in[0], &in[consumed]);
        if (start_stop) {
            // std::cout << "    publishing message with " << d_vector.size() << " items"
            // << std::endl;
            publish_message();
            if (d_skip == 0) {
                d_triggered = true;
                d_next += d_take;
            }
        }
    } else {
        // if we are not saving data but need to start next time:
        if (start_stop)
            d_triggered = true;
    }

    // Tell runtime system how many output items we produced.
    return consumed;
}

template <class T>
void take_skip_to_pdu_impl<T>::set_take(uint32_t take)
{
    gr::thread::scoped_lock l(this->d_setlock);

    d_take = take;
}


template <class T>
void take_skip_to_pdu_impl<T>::set_skip(uint32_t skip)
{
    gr::thread::scoped_lock l(this->d_setlock);

    d_skip = skip;
}

template class take_skip_to_pdu<unsigned char>;
template class take_skip_to_pdu<short>;
template class take_skip_to_pdu<int>;
template class take_skip_to_pdu<float>;
template class take_skip_to_pdu<gr_complex>;
} /* namespace pdu */
} /* namespace gr */

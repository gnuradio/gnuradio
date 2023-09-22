/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_TAGS_TO_PDU_H
#define INCLUDED_PDU_TAGS_TO_PDU_H

#include <gnuradio/pdu/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace pdu {

/*!
 * \brief Tags to PDU
 * \ingroup pdu_blk
 *
 * This block will generate a PDU (of up to Max PDU Size) based on input
 * tags. No tag alignment in input buffer is necessary.
 *
 */
template <class T>
class PDU_API tags_to_pdu : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<tags_to_pdu<T>> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of pdu_utils::tags_to_pdu.
     *
     * @param start_tag - PMT Key of the start tag
     * @param end_tag - PMT Key of the end tag
     * @param max_pdu_size - maximum number of items in PDU
     * @param samp_rate - sample rate of input data stream
     * @param prepend - data vector to prepend to the PDU
     * @param pub_start_msg - publish message when start tag is received
     * @param tail_size -
     * @param start_time - time to start burst
     */
    static sptr make(pmt::pmt_t start_tag,
                     pmt::pmt_t end_tag,
                     uint32_t max_pdu_size,
                     double samp_rate,
                     std::vector<T> prepend,
                     bool pub_start_msg,
                     uint32_t tail_size,
                     double start_time);

    virtual void set_eob_parameters(uint32_t, uint32_t) = 0;
    virtual uint32_t get_eob_offset(void) = 0;
    virtual uint32_t get_eob_alignment(void) = 0;

    virtual void set_start_tag(pmt::pmt_t tag) = 0;
    virtual void set_end_tag(pmt::pmt_t tag) = 0;
    virtual void set_time_tag_key(pmt::pmt_t tag) = 0;
    virtual void set_prepend(std::vector<T> prepend) = 0;
    virtual void set_tail_size(uint32_t size) = 0;
    virtual void set_max_pdu_size(uint32_t size) = 0;
    virtual void set_samp_rate(double) = 0;
    virtual void set_start_time(double) = 0;
    virtual void publish_start_msgs(bool) = 0;
    virtual void enable_time_debug(bool) = 0;
};

typedef tags_to_pdu<unsigned char> tags_to_pdu_b;
typedef tags_to_pdu<short> tags_to_pdu_s;
typedef tags_to_pdu<int> tags_to_pdu_i;
typedef tags_to_pdu<float> tags_to_pdu_f;
typedef tags_to_pdu<gr_complex> tags_to_pdu_c;
} // namespace pdu
} // namespace gr

#endif /* INCLUDED_PDU_TAGS_TO_PDU_H */

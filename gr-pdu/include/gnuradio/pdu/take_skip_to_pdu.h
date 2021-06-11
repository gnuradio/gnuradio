/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_TAKE_SKIP_TO_PDU_H
#define INCLUDED_PDU_TAKE_SKIP_TO_PDU_H

#include <gnuradio/pdu/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace pdu {

#ifndef MAXIMUM_PDU_SIZE
#define MAXIMUM_PDU_SIZE
#endif

/*!
 * \brief Extract periodic PDUs from a data stream.
 * \ingroup pdu_blk
 *
 * This block will generate PDUs of size TAKE skipping SKIP samples between
 *
 */
template <class T>
class PDU_API take_skip_to_pdu : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<take_skip_to_pdu<T>> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of pdu::take_skip_to_pdu.
     *
     * @param take - size of generated PDUs in samples
     * @param skip - number of samples to skip between takes
     */
    static sptr make(uint32_t take, uint32_t skip);

    virtual void set_take(uint32_t take) = 0;
    virtual void set_skip(uint32_t skip) = 0;
};

typedef take_skip_to_pdu<unsigned char> take_skip_to_pdu_b;
typedef take_skip_to_pdu<short> take_skip_to_pdu_s;
typedef take_skip_to_pdu<int> take_skip_to_pdu_i;
typedef take_skip_to_pdu<float> take_skip_to_pdu_f;
typedef take_skip_to_pdu<gr_complex> take_skip_to_pdu_c;
} // namespace pdu
} // namespace gr

#endif /* INCLUDED_PDU_TAKE_SKIP_TO_PDU_H */

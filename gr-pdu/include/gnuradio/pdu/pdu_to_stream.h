/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 *
 */

#ifndef INCLUDED_PDU_PDU_TO_STREAM_H
#define INCLUDED_PDU_PDU_TO_STREAM_H

#include <gnuradio/pdu.h>
#include <gnuradio/pdu/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace pdu {

enum early_pdu_behavior_t {
    EARLY_BURST_APPEND = 0,
    EARLY_BURST_DROP,
    EARLY_BURST_BALK,
};

/*!
 * \brief Emit a PDU data as a simple GR stream.
 *
 * The PDU to Stream block takes data stored in the uniform vector PDU
 * and emits it with UHD style tx_sob and tx_eob tags. If desired, the
 * metadata dictionary can contain a tx_time tag which will append a UHD
 * style tx_time tag to the tx_sob sample causing transmission at a well
 * defined point in time.
 *
 * \ingroup pdu_utils
 *
 */
template <class T>
class PDU_API pdu_to_stream : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<pdu_to_stream<T>> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of pdu_utils::pdu_to_stream_x.
     *
     * @param early_pdu_behavior - behavior for bursts received before previous one is
     * done processing
     * @param max_queue_size - max number of PDUs to queue
     */
    static sptr make(early_pdu_behavior_t early_pdu_behavior,
                     uint32_t max_queue_size = 64);

    /**
     * Set Max Queue size
     *
     * @param size - maximum number of queued bursts
     */
    virtual void set_max_queue_size(uint32_t size) = 0;
};

typedef pdu_to_stream<unsigned char> pdu_to_stream_b;
typedef pdu_to_stream<short> pdu_to_stream_s;
typedef pdu_to_stream<short> pdu_to_stream_i;
typedef pdu_to_stream<float> pdu_to_stream_f;
typedef pdu_to_stream<gr_complex> pdu_to_stream_c;
} // namespace pdu
} // namespace gr

#endif /* INCLUDED_PDU_PDU_TO_STREAM_H */

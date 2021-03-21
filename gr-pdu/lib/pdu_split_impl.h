/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_PDU_SPLIT_IMPL_H
#define INCLUDED_PDU_PDU_SPLIT_IMPL_H

#include <gnuradio/pdu/pdu_split.h>

namespace gr {
namespace pdu {

class pdu_split_impl : public pdu_split
{
private:
    const bool d_pass_empty_data;

    /*!
     * \brief PDU formatted messages received in this port are split into theier
     * dictionary and vector which are each emitted on separate output ports.
     *
     * \param pdu A PDU message passed from the scheduler's message handling.
     */
    void handle_pdu(const pmt::pmt_t& pdu);

public:
    pdu_split_impl(const bool pass_empty_data);
    ~pdu_split_impl() override;
};

} // namespace pdu
} // namespace gr

#endif /* INCLUDED_PDU_PDU_SPLIT_IMPL_H */

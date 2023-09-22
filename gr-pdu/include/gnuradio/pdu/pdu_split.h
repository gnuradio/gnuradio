/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_PDU_SPLIT_H
#define INCLUDED_PDU_PDU_SPLIT_H

#include <gnuradio/block.h>
#include <gnuradio/pdu/api.h>

namespace gr {
namespace pdu {

/*!
 * \brief Split PDU dict and data to separate ports
 * \ingroup debug_tools_blk
 * \ingroup pdu_blk
 *
 * Splits a PDU into its metadata dictionary and vector,
 * outputs nothing if the input message is not a PDU. Useful for stripping metadata out
 * for printing or saving.
 *
 */
class PDU_API pdu_split : virtual public gr::block
{
public:
    typedef std::shared_ptr<pdu_split> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of pdu::pdu_split.
     *
     * @param pass_empty_data - true to pass PDU vectors that are length 0
     */
    static sptr make(bool pass_empty_data = false);
};

} // namespace pdu
} // namespace gr

#endif /* INCLUDED_PDU_PDU_SPLIT_H */

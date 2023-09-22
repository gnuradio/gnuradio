/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_PDU_SET_H
#define INCLUDED_PDU_PDU_SET_H

#include <gnuradio/block.h>
#include <gnuradio/pdu/api.h>

namespace gr {
namespace pdu {

/*!
 * \brief Set k=>v in pdu's meta field and pass on
 * \ingroup message_tools_blk
 * \ingroup debug_tools_blk
 */
class PDU_API pdu_set : virtual public block
{
public:
    // gr::pdu::pdu_set::sptr
    typedef std::shared_ptr<pdu_set> sptr;

    /*!
     * \brief Construct a PDU meta set block
     */
    static sptr make(pmt::pmt_t k, pmt::pmt_t v);
    virtual void set_key(pmt::pmt_t key) = 0;
    virtual void set_val(pmt::pmt_t val) = 0;
};

} /* namespace pdu */
} /* namespace gr */

#endif /* INCLUDED_PDU_PDU_SET_H */

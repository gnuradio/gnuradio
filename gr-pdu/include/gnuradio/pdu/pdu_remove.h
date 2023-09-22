/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_PDU_REMOVE_H
#define INCLUDED_PDU_PDU_REMOVE_H

#include <gnuradio/block.h>
#include <gnuradio/pdu/api.h>

namespace gr {
namespace pdu {

/*!
 * \brief remove key k in pdu's meta field and pass on
 * \ingroup message_tools_blk
 * \ingroup debug_tools_blk
 */
class PDU_API pdu_remove : virtual public block
{
public:
    // gr::pdu::pdu_remove::sptr
    typedef std::shared_ptr<pdu_remove> sptr;

    /*!
     * \brief Construct a PDU meta remove block
     */
    static sptr make(pmt::pmt_t k);
    virtual void set_key(pmt::pmt_t key) = 0;
};

} /* namespace pdu */
} /* namespace gr */

#endif /* INCLUDED_PDU_PDU_REMOVE_H */

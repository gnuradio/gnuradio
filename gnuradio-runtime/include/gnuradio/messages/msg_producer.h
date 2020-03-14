/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_MSG_PRODUCER_H
#define INCLUDED_MSG_PRODUCER_H

#include <gnuradio/api.h>
#include <pmt/pmt.h>
#include <memory>

namespace gr {
namespace messages {

/*!
 * \brief Virtual base class that produces messages
 */
class GR_RUNTIME_API msg_producer
{
public:
    msg_producer() {}
    virtual ~msg_producer();

    /*!
     * \brief send \p msg to \p msg_producer
     */
    virtual pmt::pmt_t retrieve() = 0;
};

typedef std::shared_ptr<msg_producer> msg_producer_sptr;

} /* namespace messages */
} /* namespace gr */

#endif /* INCLUDED_MSG_PRODUCER_H */

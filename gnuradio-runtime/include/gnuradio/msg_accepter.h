/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_MSG_ACCEPTER_H
#define INCLUDED_GR_RUNTIME_MSG_ACCEPTER_H

#include <gnuradio/api.h>
#include <gnuradio/messages/msg_accepter.h>
#include <pmt/pmt.h>

namespace gr {

/*!
 * \brief Accepts messages and inserts them into a message queue,
 * then notifies subclass gr::basic_block there is a message pending.
 */
class GR_RUNTIME_API msg_accepter : public gr::messages::msg_accepter
{
public:
    msg_accepter();
    ~msg_accepter() override;

    void post(pmt::pmt_t which_port, pmt::pmt_t msg) override;
};

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_MSG_ACCEPTER_H */

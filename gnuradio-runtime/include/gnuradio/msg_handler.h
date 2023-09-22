/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_MSG_HANDLER_H
#define INCLUDED_GR_MSG_HANDLER_H

#include <gnuradio/api.h>
#include <gnuradio/message.h>

namespace gr {

class msg_handler;
typedef std::shared_ptr<msg_handler> msg_handler_sptr;

/*!
 * \brief abstract class of message handlers
 * \ingroup base
 */
class GR_RUNTIME_API msg_handler
{
public:
    virtual ~msg_handler();

    //! handle \p msg
    virtual void handle(message::sptr msg) = 0;
};

} /* namespace gr */

#endif /* INCLUDED_GR_MSG_HANDLER_H */

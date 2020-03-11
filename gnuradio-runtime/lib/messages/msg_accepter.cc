/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/messages/msg_accepter.h>

namespace gr {
namespace messages {

msg_accepter::~msg_accepter()
{
    // NOP, required as virtual destructor
}

} /* namespace messages */
} /* namespace gr */

/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/messages/msg_producer.h>

namespace gr {
namespace messages {

msg_producer::~msg_producer()
{
    // NOP, required as virtual destructor
}

} /* namespace messages */
} /* namespace gr */

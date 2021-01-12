/* -*- c++ -*- */
/*
 * Copyright 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_TERMINATE_HANDLER_H
#define INCLUDED_GR_TERMINATE_HANDLER_H

#include <gnuradio/api.h>

namespace gr {
GR_RUNTIME_API void install_terminate_handler();
}

#endif /* INCLUDED_TERMINATE_HANDLER_H */

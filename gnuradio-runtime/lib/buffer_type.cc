/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include <gnuradio/buffer_type.h>

namespace gr {

uint32_t buffer_type_base::s_nextId = 0;
std::mutex buffer_type_base::s_mutex;


} /* namespace gr */
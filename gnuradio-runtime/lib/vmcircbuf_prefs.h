/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef GR_VMCIRCBUF_PREFS_H
#define GR_VMCIRCBUF_PREFS_H

#include <gnuradio/api.h>
#include <string_view>
#include <string>

namespace gr {

class GR_RUNTIME_API vmcircbuf_prefs
{
public:
    static std::string get(std::string_view key);
    static void set(std::string_view key, std::string_view value);
};

} /* namespace gr */

#endif /* GR_VMCIRCBUF_PREFS_H */

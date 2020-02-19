/* -*- c++ -*- */
/*
 * Copyright 2003,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef GR_PREFERENCES_H
#define GR_PREFERENCES_H

#include <gnuradio/api.h>

namespace gr {

class GR_RUNTIME_API vmcircbuf_prefs
{
public:
    static int get(const char* key, char* value, int value_size);
    static void set(const char* key, const char* value);
};

} /* namespace gr */

#endif /* GR_PREFERENCES_H */

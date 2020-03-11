/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

class gr::sync_interpolator : public gr::sync_block
{
 protected:

  sync_interpolator(const std::string &name,
                    gr::io_signature::sptr input_signature,
                    gr::io_signature::sptr output_signature,
                    unsigned interpolation);
};

/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

class gr::tagged_stream_block : public gr::block
{
 protected:
  tagged_stream_block(const std::string &name,
                      gr::io_signature::sptr input_signature,
                      gr::io_signature::sptr output_signature,
                      const std::string &length_tag_key);
};


/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_MESSAGE_H
#define INCLUDED_GR_MESSAGE_H

#include <gnuradio/api.h>
#include <gnuradio/types.h>
#include <string>

namespace gr {

  /*!
   * \brief Message class.
   *
   * \ingroup misc
   * The ideas and method names for adjustable message length were
   * lifted from the click modular router "Packet" class.
   */
  class GR_RUNTIME_API message
  {
  public:
    typedef boost::shared_ptr<message> sptr;

  private:
    sptr         d_next;	// link field for msg queue
    long	 d_type;	// type of the message
    double	 d_arg1;	// optional arg1
    double 	 d_arg2;	// optional arg2

    unsigned char *d_buf_start;	// start of allocated buffer
    unsigned char *d_msg_start;	// where the msg starts
    unsigned char *d_msg_end;	// one beyond end of msg
    unsigned char *d_buf_end;	// one beyond end of allocated buffer

    message(long type, double arg1, double arg2, size_t length);

    friend class msg_queue;

    unsigned char *buf_data() const { return d_buf_start; }
    size_t buf_len() const { return d_buf_end - d_buf_start; }

  public:
    /*!
     * \brief public constructor for message
     */
    static sptr make(long type = 0, double arg1 = 0, double arg2 = 0, size_t length = 0);

    static sptr make_from_string(const std::string s, long type = 0,
                                 double arg1 = 0, double arg2 = 0);


    ~message();

    long type() const   { return d_type; }
    double arg1() const { return d_arg1; }
    double arg2() const { return d_arg2; }

    void set_type(long type)   { d_type = type; }
    void set_arg1(double arg1) { d_arg1 = arg1; }
    void set_arg2(double arg2) { d_arg2 = arg2; }

    unsigned char *msg() const { return d_msg_start; }
    size_t length() const { return d_msg_end - d_msg_start; }
    std::string to_string() const;
  };

  GR_RUNTIME_API long message_ncurrently_allocated();

} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_H */

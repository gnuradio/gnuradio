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

namespace gr {

  /*!
   * \brief Message.
   *
   * The ideas and method names for adjustable message length were
   * lifted from the click modular router "Packet" class.
   */
  class message
  {
  public:
    typedef boost::shared_ptr<message> sptr;

  private:
    message(long type, double arg1, double arg2, size_t length);

    unsigned char *buf_data() const;
    size_t buf_len() const;

  public:
    static sptr make(long type = 0, double arg1 = 0, double arg2 = 0, size_t length = 0);

    static sptr make_from_string(const std::string s, long type = 0,
                                 double arg1 = 0, double arg2 = 0);

    ~message();

    long type() const;
    double arg1() const;
    double arg2() const;

    void set_type(long type);
    void set_arg1(double arg1);
    void set_arg2(double arg2);

    size_t length() const;
    std::string to_string() const;
  };

  %rename(message_ncurrently_allocated) message_ncurrently_allocated;
  long message_ncurrently_allocated();
}

%template(message_sptr) boost::shared_ptr<gr::message>;
%pythoncode %{
message_from_string = message.make_from_string
message = message.make
%}

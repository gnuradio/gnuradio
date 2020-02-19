/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

namespace gr {

  %pythonappend message::to_string %{
    import sys
    if sys.version_info[0] > 2:
      val =  val.encode("utf8", errors="surrogateescape")
  %}

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

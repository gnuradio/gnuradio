/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
    typedef std::shared_ptr<message> sptr;

private:
    sptr d_next;   // link field for msg queue
    long d_type;   // type of the message
    double d_arg1; // optional arg1
    double d_arg2; // optional arg2

    std::vector<unsigned char> d_buf;
    unsigned char* d_msg_start; // where the msg starts
    unsigned char* d_msg_end;   // one beyond end of msg

    message(long type, double arg1, double arg2, size_t length);

    friend class msg_queue;

    unsigned char* buf_data() { return d_buf.data(); }
    size_t buf_len() const { return d_buf.size(); }

public:
    /*!
     * \brief public constructor for message
     */
    static sptr make(long type = 0, double arg1 = 0, double arg2 = 0, size_t length = 0);

    static sptr make_from_string(const std::string s,
                                 long type = 0,
                                 double arg1 = 0,
                                 double arg2 = 0);


    ~message();

    long type() const { return d_type; }
    double arg1() const { return d_arg1; }
    double arg2() const { return d_arg2; }

    void set_type(long type) { d_type = type; }
    void set_arg1(double arg1) { d_arg1 = arg1; }
    void set_arg2(double arg2) { d_arg2 = arg2; }

    unsigned char* msg() const { return d_msg_start; }
    size_t length() const { return d_msg_end - d_msg_start; }
    std::string to_string() const;
};

GR_RUNTIME_API long message_ncurrently_allocated();

} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_H */

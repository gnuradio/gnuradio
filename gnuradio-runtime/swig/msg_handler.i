/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*!
 * \brief abstract class of message handlers
 */
class gr::msg_handler
{
public:
  virtual ~msg_handler () = 0;

  //! handle \p msg
  virtual void handle(gr::message::sptr msg) = 0;
};

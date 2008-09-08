/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_USRP2_SINK_32FC_H
#define INCLUDED_USRP2_SINK_32FC_H

#include <usrp2_sink_base.h>

class usrp2_sink_32fc;
typedef boost::shared_ptr<usrp2_sink_32fc> usrp2_sink_32fc_sptr;

usrp2_sink_32fc_sptr
usrp2_make_sink_32fc() throw (std::runtime_error);

class usrp2_sink_32fc : public usrp2_sink_base {
private:

  friend usrp2_sink_32fc_sptr
  usrp2_make_sink_32fc() throw (std::runtime_error);

protected:
  usrp2_sink_32fc() throw (std::runtime_error);

public:
  ~usrp2_sink_32fc();
};

#endif /* INCLUDED_USRP2_SINK_32FC_H */

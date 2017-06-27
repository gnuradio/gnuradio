/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef RPCSERVER_BASE_H
#define RPCSERVER_BASE_H

#include <gnuradio/rpccallbackregister_base.h>

class rpcserver_base : public virtual callbackregister_base
{
public:
  rpcserver_base() : cur_priv(RPC_PRIVLVL_ALL) {;}
  virtual ~rpcserver_base() {;}

  virtual void registerConfigureCallback(const std::string &id, const configureCallback_t callback) = 0;
  virtual void unregisterConfigureCallback(const std::string &id) = 0;

  virtual void registerQueryCallback(const std::string &id, const queryCallback_t callback) = 0;
  virtual void unregisterQueryCallback(const std::string &id) = 0;

  virtual void registerHandlerCallback(const std::string &id, const handlerCallback_t callback) = 0;
  virtual void unregisterHandlerCallback(const std::string &id) = 0;

  virtual void setCurPrivLevel(const priv_lvl_t priv) { cur_priv = priv; }

  typedef boost::shared_ptr<rpcserver_base> rpcserver_base_sptr;
protected:
  priv_lvl_t cur_priv;

private:
};

#endif /* RPCSERVER_BASE_H */

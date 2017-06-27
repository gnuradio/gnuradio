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

#ifndef RPCCALLBACKREGISTER_BASE_H
#define RPCCALLBACKREGISTER_BASE_H

#include <gnuradio/messages/msg_accepter.h>
#include <gnuradio/messages/msg_producer.h>

typedef uint32_t DisplayType;

//! DisplayType Plotting types
const uint32_t DISPNULL = 0x0000;
const uint32_t DISPTIME = 0x0001;
const uint32_t DISPXY   = 0x0002;
const uint32_t DISPPSD  = 0x0004;
const uint32_t DISPSPEC = 0x0008;
const uint32_t DISPRAST = 0x0010;

//! DisplayType Options
const uint32_t DISPOPTCPLX    = 0x0100;
const uint32_t DISPOPTLOG     = 0x0200;
const uint32_t DISPOPTSTEM    = 0x0400;
const uint32_t DISPOPTSTRIP   = 0x0800;
const uint32_t DISPOPTSCATTER = 0x1000;

enum priv_lvl_t {
  RPC_PRIVLVL_ALL = 0,
  RPC_PRIVLVL_MIN = 9,
  RPC_PRIVLVL_NONE = 10
};

enum KnobType {
  KNOBBOOL,       KNOBCHAR,       KNOBINT,        KNOBFLOAT,
  KNOBDOUBLE,     KNOBSTRING,     KNOBLONG,       KNOBVECBOOL,
  KNOBCOMPLEX,    KNOBCOMPLEXD,
  KNOBVECCHAR,    KNOBVECINT,     KNOBVECFLOAT,   KNOBVECDOUBLE,
  KNOBVECSTRING,  KNOBVECLONG
};

struct callbackregister_base
{
  struct callback_base_t
  {
  public:
    callback_base_t(const priv_lvl_t priv_, const std::string& units_,
		    const DisplayType display_, const std::string& desc_,
		    const pmt::pmt_t min_, const pmt::pmt_t max_, const pmt::pmt_t def)
      : priv(priv_), units(units_), description(desc_),
	min(min_), max(max_), defaultvalue(def), display(display_)
    {
    }

    priv_lvl_t	priv;
    std::string	units, description;
    pmt::pmt_t	min, max, defaultvalue;
    DisplayType  display;
  };

  template<typename T, typename Tsptr>
  class callback_t : public callback_base_t
  {
  public:
    callback_t(T* callback_, priv_lvl_t priv_,
	       const std::string& units_, const DisplayType display_, const:: std::string& desc_,
	       const pmt::pmt_t& min_, const pmt::pmt_t& max_, const pmt::pmt_t& def_) :
      callback_base_t(priv_, units_, display_, desc_, min_, max_, def_),
      callback(callback_)
    {
    }

    callback_t(T* callback_, priv_lvl_t priv_, const:: std::string& desc_) :
      callback_base_t(priv_, "", 0, desc_, pmt::pmt_t(), pmt::pmt_t(), pmt::pmt_t()),
      callback(callback_)
    {
    }

    Tsptr callback;
  };

  typedef callback_t<gr::messages::msg_accepter, gr::messages::msg_accepter_sptr> configureCallback_t;
  typedef callback_t<gr::messages::msg_producer, gr::messages::msg_producer_sptr> queryCallback_t;
  typedef callback_t<gr::messages::msg_accepter, gr::messages::msg_accepter_sptr> handlerCallback_t;

  callbackregister_base() {;}
  virtual ~callbackregister_base() {;}

  virtual void registerConfigureCallback(const std::string &id, const configureCallback_t callback) = 0;
  virtual void unregisterConfigureCallback(const std::string &id) = 0;
  virtual void registerQueryCallback(const std::string &id, const queryCallback_t callback) = 0;
  virtual void unregisterQueryCallback(const std::string &id) = 0;
  virtual void registerHandlerCallback(const std::string &id, const handlerCallback_t callback) = 0;
  virtual void unregisterHandlerCallback(const std::string &id) = 0;
};

#endif /* RPCCALLBACKREGISTER_BASE_H */

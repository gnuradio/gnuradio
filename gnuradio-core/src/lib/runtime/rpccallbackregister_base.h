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

#include <gruel/msg_accepter.h>
#include <gruel/msg_producer.h>

enum DisplayType { 
  DISPNULL, 
  DISPTIMESERIESF,
  DISPTIMESERIESC,
  DISPXYSCATTER,
  DISPXYLINE,
  DISPLOGTIMESERIESF,
  DISPLOGTIMESERIESC,
  DISPSTRIPCHARTF,
  DISPSTRIPCHARTC,
};

enum priv_lvl_t {
  RPC_PRIVLVL_ALL = 0,
  RPC_PRIVLVL_MIN = 9,
  RPC_PRIVLVL_NONE = 10
};

enum KnobType {
  KNOBBOOL,       KNOBCHAR,       KNOBINT,        KNOBFLOAT,
  KNOBDOUBLE,     KNOBSTRING,     KNOBLONG,       KNOBVECBOOL,
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

    Tsptr callback;
  };

  typedef callback_t<gruel::msg_accepter, gruel::msg_accepter_sptr> configureCallback_t;
  typedef callback_t<gruel::msg_producer, gruel::msg_producer_sptr> queryCallback_t;

  callbackregister_base() {;}
  virtual ~callbackregister_base() {;}

  virtual void registerConfigureCallback(const std::string &id, const configureCallback_t callback) = 0;
  virtual void unregisterConfigureCallback(const std::string &id) = 0;
  virtual void registerQueryCallback(const std::string &id, const queryCallback_t callback) = 0;
  virtual void unregisterQueryCallback(const std::string &id) = 0;
};

#endif /* RPCCALLBACKREGISTER_BASE_H */

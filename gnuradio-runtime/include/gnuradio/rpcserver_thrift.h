/* -*- c++ -*- */
/*
 * Copyright 2014,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef RPCSERVER_THRIFT_H
#define RPCSERVER_THRIFT_H

#include <gnuradio/rpcserver_base.h>
#include <gnuradio/rpcpmtconverters_thrift.h>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include "thrift/ControlPort.h"
#include "thrift/gnuradio_types.h"
#include <boost/format.hpp>
#include <boost/thread/mutex.hpp>

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)

class rpcserver_thrift : public virtual rpcserver_base, public GNURadio::ControlPortIf
{
public:
  rpcserver_thrift();
  virtual ~rpcserver_thrift();

  void registerConfigureCallback(const std::string &id,
                                 const configureCallback_t callback);
  void unregisterConfigureCallback(const std::string &id);

  void registerQueryCallback(const std::string &id,
                             const queryCallback_t callback);
  void unregisterQueryCallback(const std::string &id);

  void setKnobs(const GNURadio::KnobMap&);
  void getKnobs(GNURadio::KnobMap&,
                const GNURadio::KnobIDList&);
  void getRe(GNURadio::KnobMap&,
             const GNURadio::KnobIDList&);
  void properties(GNURadio::KnobPropMap&,
                  const GNURadio::KnobIDList& knobs);
  virtual void shutdown();

 private:
  boost::mutex d_callback_map_lock;

  typedef std::map<std::string, configureCallback_t> ConfigureCallbackMap_t;
  ConfigureCallbackMap_t d_setcallbackmap;

  typedef std::map<std::string, queryCallback_t> QueryCallbackMap_t;
  QueryCallbackMap_t d_getcallbackmap;

  template<typename T, typename TMap> struct set_f
    : public std::unary_function<T,void>
  {
    set_f(TMap &_setcallbackmap, const priv_lvl_t &_cur_priv)
      : d_setcallbackmap(_setcallbackmap), cur_priv(_cur_priv)
    {
      ;
    }

    void operator()(const T& p)
    {
      ConfigureCallbackMap_t::const_iterator iter(d_setcallbackmap.find(p.first));
      if(iter != d_setcallbackmap.end()) {
        if(cur_priv <= iter->second.priv) {
          (*iter->second.callback).post(pmt::PMT_NIL, rpcpmtconverter::To_PMT::instance(p.second));
        }
        else {
          std::cout << "Key " << p.first << " requires PRIVLVL <= "
                    << iter->second.priv << " to set, currently at: "
                    << cur_priv << std::endl;
        }
      }
      else {
        throw apache::thrift::TApplicationException(__FILE__ " " S__LINE__);
      }
    }

    TMap& d_setcallbackmap;
    const priv_lvl_t& cur_priv;
  };

  template<typename T, typename TMap>
  struct get_f : public std::unary_function<T,void>
  {
    get_f(TMap &_getcallbackmap, const priv_lvl_t &_cur_priv, GNURadio::KnobMap &_outknobs) :
      d_getcallbackmap(_getcallbackmap), cur_priv(_cur_priv), outknobs(_outknobs)
    {}

    void operator()(const T& p)
    {
      QueryCallbackMap_t::const_iterator iter(d_getcallbackmap.find(p));
      if(iter != d_getcallbackmap.end()) {
        if(cur_priv <= iter->second.priv) {
          outknobs[p] = rpcpmtconverter::from_pmt((*iter->second.callback).retrieve());
        }
        else {
          std::cout << "Key " << iter->first << " requires PRIVLVL: <= "
                    << iter->second.priv << " to get, currently at: "
                    << cur_priv << std::endl;
        }
      }
      else {
        std::stringstream ss;
        ss << "Ctrlport Key called with unregistered key (" << p << ")\n";
        std::cout << ss.str();
        throw apache::thrift::TApplicationException(__FILE__ " " S__LINE__);
      }
    }

    TMap& d_getcallbackmap;
    const priv_lvl_t& cur_priv;
    GNURadio::KnobMap& outknobs;
  };

  template<typename T, typename TMap, typename TKnobMap>
  struct get_all_f : public std::unary_function<T,void>
  {
    get_all_f(TMap &_getcallbackmap, const priv_lvl_t &_cur_priv, TKnobMap &_outknobs) :
      d_getcallbackmap(_getcallbackmap), cur_priv(_cur_priv), outknobs(_outknobs)
    {;}

    void operator()(const T& p)
    {
      if(cur_priv <= p.second.priv) {
        outknobs[p.first] = rpcpmtconverter::from_pmt(p.second.callback->retrieve());
      }
      else {
        std::cout << "Key " << p.first << " requires PRIVLVL <= "
                  << p.second.priv << " to get, currently at: "
                  << cur_priv << std::endl;
      }
    }

    TMap& d_getcallbackmap;
    const priv_lvl_t& cur_priv;
    TKnobMap& outknobs;
  };

  template<typename T, typename TMap, typename TKnobMap>
  struct properties_all_f : public std::unary_function<T,void>
  {
    properties_all_f(QueryCallbackMap_t &_getcallbackmap,
                     const priv_lvl_t &_cur_priv,
                     GNURadio::KnobPropMap &_outknobs)
      : d_getcallbackmap(_getcallbackmap),
        cur_priv(_cur_priv),
        outknobs(_outknobs)
    {;}

    void operator()(const T& p)
    {
      if(cur_priv <= p.second.priv) {
        GNURadio::KnobProp prop;
        prop.type  = GNURadio::KnobType::KNOBDOUBLE;
        prop.units = p.second.units;
        prop.description = p.second.description;
        prop.min   = rpcpmtconverter::from_pmt(p.second.min);
        prop.max   = rpcpmtconverter::from_pmt(p.second.max);
        prop.display = static_cast<uint32_t>(p.second.display);
        outknobs[p.first] = prop;
      }
      else {
        std::cout << "Key " << p.first << " requires PRIVLVL <= "
                  << p.second.priv << " to get, currently at: "
                  << cur_priv << std::endl;
      }
    }

    TMap& d_getcallbackmap;
    const priv_lvl_t& cur_priv;
    TKnobMap& outknobs;
  };

  template<class T, typename TMap, typename TKnobMap>
  struct properties_f : public std::unary_function<T,void>
  {
    properties_f(TMap &_getcallbackmap, const priv_lvl_t &_cur_priv, TKnobMap &_outknobs) :
      d_getcallbackmap(_getcallbackmap), cur_priv(_cur_priv), outknobs(_outknobs)
    {;}

    void operator()(const T& p)
    {
      typename TMap::const_iterator iter(d_getcallbackmap.find(p));
      if(iter != d_getcallbackmap.end()) {
        if(cur_priv <= iter->second.priv) {
          GNURadio::KnobProp prop;
          prop.type  = GNURadio::KnobType::KNOBDOUBLE;
          prop.units = iter->second.units;
          prop.description = iter->second.description;
          prop.min   = rpcpmtconverter::from_pmt(iter->second.min);
          prop.max   = rpcpmtconverter::from_pmt(iter->second.max);
          prop.display = static_cast<uint32_t>(iter->second.display);
          outknobs[p] = prop;
        }
        else {
          std::cout << "Key " << iter->first << " requires PRIVLVL: <= "
                    << iter->second.priv << " to get, currently at: " << cur_priv << std::endl;
        }
      }
      else {
        throw apache::thrift::TApplicationException(__FILE__ " " S__LINE__);
      }
    }

    TMap& d_getcallbackmap;
    const priv_lvl_t& cur_priv;
    TKnobMap& outknobs;
  };
};

#endif /* RPCSERVER_THRIFT_H */

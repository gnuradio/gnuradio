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

#include <gnuradio/rpcserver_thrift.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <pmt/pmt.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <boost/xpressive/xpressive.hpp>
#include "thrift/ControlPort.h"

#define DEBUG 0

using namespace rpcpmtconverter;

rpcserver_thrift::rpcserver_thrift()
{
  //std::cerr << "rpcserver_thrift::ctor" << std::endl;
}

rpcserver_thrift::~rpcserver_thrift()
{
  //std::cerr << "rpcserver_thrift::dtor" << std::endl;
}

void
rpcserver_thrift::registerConfigureCallback(const std::string &id,
                                            const configureCallback_t callback)
{
  boost::mutex::scoped_lock lock(d_callback_map_lock);
  {
    ConfigureCallbackMap_t::const_iterator iter(d_setcallbackmap.find(id));
    if(iter != d_setcallbackmap.end()) {
      std::stringstream s;
      s << "rpcserver_thrift:: rpcserver_thrift ERROR registering set, already registered: "
        << id << std::endl;
      throw std::runtime_error(s.str().c_str());
    }
  }

  if(DEBUG) {
    std::cerr << "rpcserver_thrift registering set: " << id << std::endl;
  }
  d_setcallbackmap.insert(ConfigureCallbackMap_t::value_type(id, callback));
}

void
rpcserver_thrift::unregisterConfigureCallback(const std::string &id)
{
  boost::mutex::scoped_lock lock(d_callback_map_lock);
  ConfigureCallbackMap_t::iterator iter(d_setcallbackmap.find(id));
  if(iter == d_setcallbackmap.end()) {
    std::stringstream s;
    s << "rpcserver_thrift:: rpcserver_thrift ERROR unregistering set, not registered: "
      << id << std::endl;
    throw std::runtime_error(s.str().c_str());
  }

  if(DEBUG)
    std::cerr << "rpcserver_thrift unregistering set: " << id << std::endl;

  d_setcallbackmap.erase(iter);
}

void
rpcserver_thrift::registerQueryCallback(const std::string &id,
                                        const queryCallback_t callback)
{
  boost::mutex::scoped_lock lock(d_callback_map_lock);
  {
    QueryCallbackMap_t::const_iterator iter(d_getcallbackmap.find(id));
    if(iter != d_getcallbackmap.end()) {
      std::stringstream s;
      s << "rpcserver_thrift:: rpcserver_thrift ERROR registering get, already registered: "
        << id << std::endl;
      throw std::runtime_error(s.str().c_str());
    }
  }

  if(DEBUG) {
    std::cerr << "rpcserver_thrift registering get: " << id << std::endl;
  }
  d_getcallbackmap.insert(QueryCallbackMap_t::value_type(id, callback));
}

void
rpcserver_thrift::unregisterQueryCallback(const std::string &id)
{
  boost::mutex::scoped_lock lock(d_callback_map_lock);
  QueryCallbackMap_t::iterator iter(d_getcallbackmap.find(id));
  if(iter == d_getcallbackmap.end()) {
    std::stringstream s;
    s << "rpcserver_thrift:: rpcserver_thrift ERROR unregistering get,  registered: "
      << id << std::endl;
    throw std::runtime_error(s.str().c_str());
  }

  if(DEBUG) {
    std::cerr << "rpcserver_thrift unregistering get: " << id << std::endl;
  }

  d_getcallbackmap.erase(iter);
}



void
rpcserver_thrift::registerHandlerCallback(const std::string &id,
                                          const handlerCallback_t callback)
{
  boost::mutex::scoped_lock lock(d_callback_map_lock);
  {
    HandlerCallbackMap_t::const_iterator iter(d_handlercallbackmap.find(id));
    if(iter != d_handlercallbackmap.end()) {
      std::stringstream s;
      s << "rpcserver_thrift:: rpcserver_thrift ERROR registering handler, already registered: "
        << id << std::endl;
      throw std::runtime_error(s.str().c_str());
    }
  }

  if(DEBUG) {
    std::cerr << "rpcserver_thrift registering handler: " << id << std::endl;
  }
  d_handlercallbackmap.insert(HandlerCallbackMap_t::value_type(id, callback));
}

void
rpcserver_thrift::unregisterHandlerCallback(const std::string &id)
{
  boost::mutex::scoped_lock lock(d_callback_map_lock);
  HandlerCallbackMap_t::iterator iter(d_handlercallbackmap.find(id));
  if(iter == d_handlercallbackmap.end()) {
    std::stringstream s;
    s << "rpcserver_thrift:: rpcserver_thrift ERROR unregistering handler, registered: "
      << id << std::endl;
    throw std::runtime_error(s.str().c_str());
  }

  if(DEBUG) {
    std::cerr << "rpcserver_thrift unregistering handler: " << id << std::endl;
  }

  d_handlercallbackmap.erase(iter);
}




void
rpcserver_thrift::setKnobs(const GNURadio::KnobMap& knobs)
{
  boost::mutex::scoped_lock lock(d_callback_map_lock);
  std::for_each(knobs.begin(), knobs.end(),
                set_f<GNURadio::KnobMap::value_type,ConfigureCallbackMap_t>
                (d_setcallbackmap, cur_priv));
}

void
rpcserver_thrift::getKnobs(GNURadio::KnobMap& _return,
                           const GNURadio::KnobIDList& knobs)
{
  boost::mutex::scoped_lock lock(d_callback_map_lock);
  if(knobs.size() == 0) {
    std::for_each(d_getcallbackmap.begin(), d_getcallbackmap.end(),
                  get_all_f<QueryCallbackMap_t::value_type, QueryCallbackMap_t, GNURadio::KnobMap>
                  (d_getcallbackmap, cur_priv, _return));
  }
  else {
    std::for_each(knobs.begin(), knobs.end(),
                  get_f<GNURadio::KnobIDList::value_type, QueryCallbackMap_t>
                  (d_getcallbackmap, cur_priv, _return));
  }
}

void
rpcserver_thrift::getRe(GNURadio::KnobMap& _return, const GNURadio::KnobIDList& knobs)
{
  boost::mutex::scoped_lock lock(d_callback_map_lock);
  if(knobs.size() == 0) {
    std::for_each(d_getcallbackmap.begin(), d_getcallbackmap.end(),
                  get_all_f<QueryCallbackMap_t::value_type, QueryCallbackMap_t, GNURadio::KnobMap>
                  (d_getcallbackmap, cur_priv, _return));
  }
  else {
    QueryCallbackMap_t::iterator it;
    for(it = d_getcallbackmap.begin(); it != d_getcallbackmap.end(); it++){
      for(size_t j=0; j<knobs.size(); j++) {
        const boost::xpressive::sregex re(boost::xpressive::sregex::compile(knobs[j]));
        if(boost::xpressive::regex_match(it->first, re)) {
          get_f<GNURadio::KnobIDList::value_type, QueryCallbackMap_t>
            (d_getcallbackmap, cur_priv, _return)(it->first);
          break;
        }
      }
    }
  }
}

void
rpcserver_thrift::properties(GNURadio::KnobPropMap& _return,
                             const GNURadio::KnobIDList& knobs)
{
  boost::mutex::scoped_lock lock(d_callback_map_lock);
  if(knobs.size() == 0) {
    std::for_each(d_getcallbackmap.begin(), d_getcallbackmap.end(),
                  properties_all_f<QueryCallbackMap_t::value_type,
                  QueryCallbackMap_t, GNURadio::KnobPropMap>(d_getcallbackmap,
                                                             cur_priv, _return));
  }
  else {
    std::for_each(knobs.begin(), knobs.end(),
                  properties_f<GNURadio::KnobIDList::value_type,
                  QueryCallbackMap_t, GNURadio::KnobPropMap>(d_getcallbackmap,
                                                             cur_priv, _return));
  }
}


void
rpcserver_thrift::postMessage(const std::string& alias,
                              const std::string& port,
                              const std::string& msg)
{
  // alias and port are received as serialized PMT strings and need to
  // be deserialized into PMTs and then the actual info from there.
  // The actual message (msg) is also received as a serialized PMT. We
  // just need to get the PMT itself out of this to pass to the set_h
  // function for handling the message post.

  boost::mutex::scoped_lock lock(d_callback_map_lock);

  pmt::pmt_t alias_pmt = pmt::deserialize_str(alias);
  pmt::pmt_t port_pmt  = pmt::deserialize_str(port);
  pmt::pmt_t msg_pmt  = pmt::deserialize_str(msg);
  std::string alias_str = pmt::symbol_to_string(alias_pmt);
  std::string port_str  = pmt::symbol_to_string(port_pmt);
  std::string iface = alias_str + "::" + port_str;

  HandlerCallbackMap_t::iterator itr = d_handlercallbackmap.begin();
  for(; itr != d_handlercallbackmap.end(); itr++) {
    if(iface == (*itr).first) {
      set_h((*itr).second, cur_priv, port_pmt, msg_pmt);
    }
  }
}

void
rpcserver_thrift::shutdown() {
  if (DEBUG) {
    std::cerr << "Shutting down..." << std::endl;
  }
}

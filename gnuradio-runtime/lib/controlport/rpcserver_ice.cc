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

#include <gnuradio/rpcserver_ice.h>
#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <pmt/pmt.h>
#include <boost/xpressive/xpressive.hpp>

#define DEBUG 0

using namespace rpcpmtconverter;

rpcserver_ice::rpcserver_ice()
{}

rpcserver_ice::~rpcserver_ice()
{}

void
rpcserver_ice::registerConfigureCallback(const std::string &id,
					 const configureCallback_t callback)
{
  {
    ConfigureCallbackMap_t::const_iterator iter(d_setcallbackmap.find(id));
    if(iter != d_setcallbackmap.end()) {
      std::stringstream s;
      s << "rpcserver_ice:: rpcserver_ice ERROR registering set, already registered: "
	<< id << std::endl;
      throw std::runtime_error(s.str().c_str());
    }
  }

  if(DEBUG)
    std::cout << "rpcserver_ice registering set: " << id << std::endl;
 
  d_setcallbackmap.insert(ConfigureCallbackMap_t::value_type(id, callback));
}

void
rpcserver_ice::unregisterConfigureCallback(const std::string &id)
{
  ConfigureCallbackMap_t::iterator iter(d_setcallbackmap.find(id));
  if(iter == d_setcallbackmap.end()) {
    std::stringstream s;
    s << "rpcserver_ice:: rpcserver_ice ERROR unregistering set, not registered: "
      << id << std::endl;
    throw std::runtime_error(s.str().c_str());
  }

  if(DEBUG)
    std::cout << "rpcserver_ice unregistering set: " << id << std::endl;

  d_setcallbackmap.erase(iter);
}

void
rpcserver_ice::registerQueryCallback(const std::string &id,
				     const queryCallback_t callback)
{
  {
    QueryCallbackMap_t::const_iterator iter(d_getcallbackmap.find(id));
    if(iter != d_getcallbackmap.end()) {
      std::stringstream s;
      s << "rpcserver_ice:: rpcserver_ice ERROR registering get, already registered: "
	<< id << std::endl;
      throw std::runtime_error(s.str().c_str());
    }
  }

  if(DEBUG)
    std::cout << "rpcserver_ice registering get: " << id << std::endl;

  d_getcallbackmap.insert(QueryCallbackMap_t::value_type(id, callback));
}

void
rpcserver_ice::unregisterQueryCallback(const std::string &id)
{
  QueryCallbackMap_t::iterator iter(d_getcallbackmap.find(id));
  if(iter == d_getcallbackmap.end()) {
    std::stringstream s;
    s << "rpcserver_ice:: rpcserver_ice ERROR unregistering get,  registered: "
      << id << std::endl;
    throw std::runtime_error(s.str().c_str());
  }

  if(DEBUG)
    std::cout << "rpcserver_ice unregistering get: " << id << std::endl;

  d_getcallbackmap.erase(iter);
}

void
rpcserver_ice::set(const GNURadio::KnobMap& knobs, const Ice::Current& c)
{
  std::for_each(knobs.begin(), knobs.end(),
		set_f<GNURadio::KnobMap::value_type,ConfigureCallbackMap_t>
		(c, d_setcallbackmap, cur_priv));
}

GNURadio::KnobMap
rpcserver_ice::getRe(const GNURadio::KnobIDList& knobs, const Ice::Current& c)
{
  GNURadio::KnobMap outknobs;

  if(knobs.size() == 0) {
    std::for_each(d_getcallbackmap.begin(), d_getcallbackmap.end(),
         get_all_f<QueryCallbackMap_t::value_type, QueryCallbackMap_t, GNURadio::KnobMap>
         (c, d_getcallbackmap, cur_priv, outknobs));
  }
  else {
    QueryCallbackMap_t::iterator it;
    for(it = d_getcallbackmap.begin(); it != d_getcallbackmap.end(); it++){
        for(size_t j=0; j<knobs.size(); j++){
            const boost::xpressive::sregex re(boost::xpressive::sregex::compile(knobs[j]));
            if(boost::xpressive::regex_match(it->first, re)){
               get_f<GNURadio::KnobIDList::value_type, QueryCallbackMap_t>
                   (c, d_getcallbackmap, cur_priv, outknobs)(it->first);
                break;
            }
        }
    }
  }
  return outknobs;
}

GNURadio::KnobMap
rpcserver_ice::get(const GNURadio::KnobIDList& knobs, const Ice::Current& c)
{
  GNURadio::KnobMap outknobs;

  if(knobs.size() == 0) {
    std::for_each(d_getcallbackmap.begin(), d_getcallbackmap.end(),
		  get_all_f<QueryCallbackMap_t::value_type, QueryCallbackMap_t, GNURadio::KnobMap>
		  (c, d_getcallbackmap, cur_priv, outknobs));
  }
  else {
    std::for_each(knobs.begin(), knobs.end(),
		  get_f<GNURadio::KnobIDList::value_type, QueryCallbackMap_t>
		  (c, d_getcallbackmap, cur_priv, outknobs));
  }
  return outknobs;
}

GNURadio::KnobPropMap
rpcserver_ice::properties(const GNURadio::KnobIDList& knobs, const Ice::Current& c)
{
  GNURadio::KnobPropMap outknobs;

  if(knobs.size() == 0) {
    std::for_each(d_getcallbackmap.begin(), d_getcallbackmap.end(),
		  properties_all_f<QueryCallbackMap_t::value_type,
		  QueryCallbackMap_t,GNURadio::KnobPropMap>(c, d_getcallbackmap, cur_priv, outknobs));
  }
  else {
    std::for_each(knobs.begin(), knobs.end(),
		  properties_f<GNURadio::KnobIDList::value_type,
		  QueryCallbackMap_t, GNURadio::KnobPropMap>(c, d_getcallbackmap, cur_priv, outknobs));
  }
  return outknobs;
}

void
rpcserver_ice::shutdown(const Ice::Current& c)
{
  if(DEBUG)
    std::cout << "Shutting down..." << std::endl;
  c.adapter->getCommunicator()->shutdown();
}

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

#ifndef RPCSERVER_ICE_H
#define RPCSERVER_ICE_H

#include <gnuradio/rpcserver_base.h>
#include <gnuradio/rpcpmtconverters_ice.h>
#include <string>
#include <sstream>
#include <map>
#include <gnuradio.h>
#include <Ice/Exception.h>
#include <boost/format.hpp>

class rpcserver_ice : public virtual rpcserver_base, public GNURadio::ControlPort
{
public:
  rpcserver_ice();
  virtual ~rpcserver_ice();

  void registerConfigureCallback(const std::string &id, const configureCallback_t callback);
  void unregisterConfigureCallback(const std::string &id);

  void registerQueryCallback(const std::string &id, const queryCallback_t callback);
  void unregisterQueryCallback(const std::string &id);

  virtual void set(const GNURadio::KnobMap&, const Ice::Current&);

  GNURadio::KnobMap get(const GNURadio::KnobIDList&, const Ice::Current&);

  GNURadio::KnobMap getRe(const GNURadio::KnobIDList&, const Ice::Current&);

  GNURadio::KnobPropMap properties(const GNURadio::KnobIDList&, const Ice::Current&);

  virtual void shutdown(const Ice::Current&);

private:
  typedef std::map<std::string, configureCallback_t> ConfigureCallbackMap_t;
  ConfigureCallbackMap_t d_setcallbackmap;

  typedef std::map<std::string, queryCallback_t> QueryCallbackMap_t;
  QueryCallbackMap_t d_getcallbackmap;

  template<typename T, typename TMap> struct set_f
    : public std::unary_function<T,void> 
  {
    set_f(const Ice::Current& _c, TMap& _setcallbackmap, const priv_lvl_t& _cur_priv) :
      c(_c), d_setcallbackmap(_setcallbackmap), cur_priv(_cur_priv)
    {;}

    void operator()(const T& p)
    {
      ConfigureCallbackMap_t::const_iterator iter(d_setcallbackmap.find(p.first));
      if(iter != d_setcallbackmap.end()) {
	if(cur_priv <= iter->second.priv) {
          (*iter->second.callback).post(pmt::PMT_NIL, rpcpmtconverter::to_pmt(p.second,c));
	}
	else {
	  std::cout << "Key " << p.first << " requires PRIVLVL <= "
		    << iter->second.priv << " to set, currently at: "
		    << cur_priv << std::endl;
	}
      }
      else {
	throw IceUtil::NullHandleException(__FILE__, __LINE__);
      }
    }

    const Ice::Current& c;
    TMap& d_setcallbackmap;
    const priv_lvl_t& cur_priv;
  };

  template<typename T, typename TMap>
  struct get_f : public std::unary_function<T,void>
  {
    get_f(const Ice::Current& _c, TMap& _getcallbackmap,
	  const priv_lvl_t& _cur_priv, GNURadio::KnobMap& _outknobs) :
      c(_c), d_getcallbackmap(_getcallbackmap), cur_priv(_cur_priv), outknobs(_outknobs)
    {}
    
    void operator()(const T& p)
    {
      QueryCallbackMap_t::const_iterator iter(d_getcallbackmap.find(p));
      if(iter != d_getcallbackmap.end()) {
	if(cur_priv <= iter->second.priv) {
	  outknobs[p] = rpcpmtconverter::from_pmt((*iter->second.callback).retrieve(), c);
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
        throw IceUtil::IllegalArgumentException(__FILE__,__LINE__,ss.str().c_str());
      }
    }

    const Ice::Current& c;
    TMap& d_getcallbackmap;
    const priv_lvl_t& cur_priv;
    GNURadio::KnobMap& outknobs;
  };

  template<typename T, typename TMap, typename TKnobMap>
  struct get_all_f : public std::unary_function<T,void>
  {
    get_all_f(const Ice::Current& _c, TMap& _getcallbackmap,
	      const priv_lvl_t& _cur_priv, TKnobMap& _outknobs) :
      c(_c), d_getcallbackmap(_getcallbackmap), cur_priv(_cur_priv), outknobs(_outknobs)
    {;}

    void operator()(const T& p)
    {
      if(cur_priv <= p.second.priv) {
	outknobs[p.first] = rpcpmtconverter::from_pmt(p.second.callback->retrieve(), c);
      }
      else {
	std::cout << "Key " << p.first << " requires PRIVLVL <= "
		  << p.second.priv << " to get, currently at: "
		  << cur_priv << std::endl;
      }
    }

    const Ice::Current& c;
    TMap& d_getcallbackmap;
    const priv_lvl_t& cur_priv;
    TKnobMap& outknobs;
  };

  template<typename T, typename TMap, typename TKnobMap>
  struct properties_all_f : public std::unary_function<T,void>
  {
    properties_all_f(const Ice::Current& _c, QueryCallbackMap_t& _getcallbackmap,
		     const priv_lvl_t& _cur_priv, GNURadio::KnobPropMap& _outknobs) :
      c(_c), d_getcallbackmap(_getcallbackmap), cur_priv(_cur_priv), outknobs(_outknobs)
    {;}

    void operator()(const T& p)
    {
      if(cur_priv <= p.second.priv) {
	GNURadio::KnobProp prop;//(new GNURadio::KnobProp());
	prop.type  = GNURadio::KNOBDOUBLE;
	prop.units = p.second.units;
	prop.description = p.second.description;
	prop.min   = rpcpmtconverter::from_pmt(p.second.min, c);
	prop.max   = rpcpmtconverter::from_pmt(p.second.max, c);
	prop.display = static_cast<uint32_t>(p.second.display);
	outknobs[p.first] = prop;
      }
      else {
	std::cout << "Key " << p.first << " requires PRIVLVL <= "
		  << p.second.priv << " to get, currently at: "
		  << cur_priv << std::endl;
      }
    }

    const Ice::Current& c;
    TMap& d_getcallbackmap;
    const priv_lvl_t& cur_priv;
    TKnobMap& outknobs;
  };

  template<class T, typename TMap, typename TKnobMap>
  struct properties_f : public std::unary_function<T,void>
  {
    properties_f(const Ice::Current& _c, TMap& _getcallbackmap,
		 const priv_lvl_t& _cur_priv, TKnobMap& _outknobs) :
      c(_c), d_getcallbackmap(_getcallbackmap), cur_priv(_cur_priv), outknobs(_outknobs)
    {;}

    void operator()(const T& p)
    {
      typename TMap::const_iterator iter(d_getcallbackmap.find(p));
      if(iter != d_getcallbackmap.end()) {
	if(cur_priv <= iter->second.priv) {
	  GNURadio::KnobProp prop;
	  prop.type  = GNURadio::KNOBDOUBLE;
	  prop.units = iter->second.units;
	  prop.description = iter->second.description;
	  prop.min   = rpcpmtconverter::from_pmt(iter->second.min, c);
	  prop.max   = rpcpmtconverter::from_pmt(iter->second.max, c);
	  prop.display = static_cast<uint32_t>(iter->second.display);
	  //outknobs[iter->first] = prop;
	  outknobs[p] = prop;
	}
	else {
	  std::cout << "Key " << iter->first << " requires PRIVLVL: <= " <<
	    iter->second.priv << " to get, currently at: " << cur_priv << std::endl;
	}
      }
      else {
	throw IceUtil::NullHandleException(__FILE__, __LINE__);
      }
    }

    const Ice::Current& c;
    TMap& d_getcallbackmap;
    const priv_lvl_t& cur_priv;
    TKnobMap& outknobs;
  };
};

#endif /* RPCSERVER_ICE_H */

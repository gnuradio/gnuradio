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

#include <rpcserver_resource.h>
#include <rpcpmtconverters_resource.h>
#include <iostream>

rpcserver_resource::rpcserver_resource()
{;}

rpcserver_resource::~rpcserver_resource()
{;}

char*
rpcserver_resource::identifier()
{
#warning "Code missing in function <char* rpcserver_resource::identifier()>"
}

void
rpcserver_resource::start()
{
#warning "Code missing in function <void rpcserver_resource::start()>"
}

void
rpcserver_resource::stop()
{
#warning "Code missing in function <void rpcserver_resource::stop()>"
}

void
rpcserver_resource::initialize()
{
#warning "Code missing in function <void rpcserver_resource::initialize()>"
}

void
rpcserver_resource::releaseObject()
{
#warning "Code missing in function <void rpcserver_resource::releaseObject()>"
}

void
rpcserver_resource::runTest(::CORBA::ULong testid, CF::Properties& testValues)
{
#warning "Code missing in function <void rpcserver_resource::runTest(::CORBA::ULong testid, CF::Properties& testValues)>"
}

void
rpcserver_resource::registerConfigureCallback(const std::string &id, const configureCallback_t callback)
{
  std::cout << "rpcserver_resource::registering: " << id << std::endl;
  d_callbackmap.insert(ConfigureCallbackMap_t::value_type(id, callback));
}

void
rpcserver_resource::configure(const CF::Properties& configProperties)
{
  for(unsigned int i=0;i<configProperties.length();++i) {
    std::string id((const char*) configProperties[i].id);
    ConfigureCallbackMap_t::const_iterator iter = d_setcallbackmap.begin();
    iter = d_setcallbackmap.find(id);
    if(iter != d_setcallbackmap.end()) {
      (*iter->second).post(rpcpmtconverter::to_pmt(configProperties[i].value));
    }
    else {
      throw IceUtil::NullHandleException(__FILE__, __LINE__);
    }
  }
}

//template<class T> struct rpcserver_resource::extractQuery : public std::unary_function<T, void> {
//    void operator()(T& x) {
//        x++;
//    }
//};

void
rpcserver_resource::query(CF::Properties& configProperties)
{
  if(configProperties.length() == 0) {
    //std::for_each(d_getcallbackmap.begin(), d_getcallbackmap.end(), extractQuery<getCallback_t>());
    configProperties.length(d_getcallbackmap.size());

  }
  else {
    for(unsigned int i=0;i<configProperties.length();++i) {
      std::string id((const char*) configProperties[i].id);
      QueryCallbackMap_t::const_iterator iter = d_getcallbackmap.begin();
      iter = d_getcallbackmap.find(id);
      if (iter != d_getcallbackmap.end()) {
	//(*iter->second).post(rpcpmtconverter::to_pmt(configProperties[i].value));
	= from_pmt((*iter->second.callback).retrieve(), c);
      }
      else {
	throw IceUtil::NullHandleException(__FILE__, __LINE__);
      }
    }
  }
}

CORBA::Object_ptr
rpcserver_resource::getPort(const char* name)
{
#warning "Code missing in function <CORBA::Object_ptr rpcserver_resource::getPort(const char* name)>"
}

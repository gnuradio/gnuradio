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

#ifndef ICE_SERVER_TEMPLATE_H
#define ICE_SERVER_TEMPLATE_H

#include <gnuradio/rpcserver_ice.h>
#include <gnuradio/ice_application_base.h>
#include <iostream>

template<typename TserverBase, typename TserverClass, typename TImplClass, typename TIceClass>
class ice_server_template : public ice_application_base<TserverBase, TImplClass>
{
public:
  ice_server_template(TImplClass* _this,
		      const std::string& contolPortName,
		      const std::string& endpointName);
  ~ice_server_template();
    
protected:
  //virtual bool application_started();
  TserverBase* i_impl();
  friend class ice_application_base<TserverBase, TImplClass>;

private:
  //virtual int run_impl(int, char*[]);
  Ice::ObjectAdapterPtr d_adapter;
  TserverBase* d_iceserver;
  const std::string d_contolPortName, d_endpointName;
};

template<typename TserverBase, typename TserverClass, typename TImplClass, typename TIceClass>
ice_server_template<TserverBase, TserverClass, TImplClass, TIceClass>::ice_server_template
    (TImplClass* _this, const std::string& controlPortName, const std::string& endpointName) 
  : ice_application_base<TserverBase, TImplClass>(_this),
    d_iceserver(0),
    d_contolPortName(controlPortName), 
    d_endpointName(endpointName)
{;}

template<typename TserverBase, typename TserverClass, typename TImplClass, typename TIceClass>
ice_server_template<TserverBase, TserverClass,TImplClass, TIceClass>::~ice_server_template()
{
  if(d_adapter) {
    d_adapter->deactivate();
    delete(d_iceserver);
    d_adapter = 0;
  }
}

template<typename TserverBase, typename TserverClass, typename TImplClass, typename TIceClass>
TserverBase* ice_server_template<TserverBase, TserverClass, TImplClass, TIceClass>::i_impl()
{ 
  if(ice_application_base<TserverBase, TImplClass>::d_this->reacquire_sync()) {
    d_adapter = (ice_application_base<TserverBase, TImplClass>::d_this->have_ice_config()) ?
      ice_application_base<TserverBase, TImplClass>::d_this->d_this->d_application->communicator()->createObjectAdapter(d_contolPortName) :
      ice_application_base<TserverBase, TImplClass>::d_this->d_this->d_application->communicator()->createObjectAdapterWithEndpoints(d_contolPortName,"tcp -h *");

    TserverClass* server_ice(new TserverClass());
    TIceClass obj(server_ice);
  
    Ice::Identity id(ice_application_base<TserverBase, TImplClass>::d_this->d_this->d_application->communicator()->stringToIdentity(d_endpointName));
    d_adapter->add(obj, id);
    d_adapter->activate();
    ice_application_base<TserverBase, TImplClass>::d_this->set_endpoint(ice_application_common::communicator()->proxyToString(d_adapter->createDirectProxy(id)));

    std::cout << std::endl << "Ice Radio Endpoint: "
	      << ice_server_template<TserverBase, TserverClass, TImplClass, TIceClass>::endpoints()[0]
	      << std::endl;

    d_iceserver =  (TserverBase*) server_ice;
    ice_application_base<TserverBase, TImplClass>::d_this->sync_reacquire();
  }

  return d_iceserver;
}

#endif /* ICE_SERVER_TEMPLATE_H */

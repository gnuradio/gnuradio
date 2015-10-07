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

#ifndef RPCSERVER_AGGREGATOR_H
#define RPCSERVER_AGGREGATOR_H

#include <vector>
#include <string>
#include <gnuradio/rpcserver_base.h>
#include <gnuradio/rpcmanager_base.h>

class rpcserver_aggregator : public virtual rpcserver_base
{
public:
  rpcserver_aggregator();
  virtual ~rpcserver_aggregator();

  void registerConfigureCallback(const std::string &id, const configureCallback_t callback);
  void unregisterConfigureCallback(const std::string &id);

  void registerQueryCallback(const std::string &id, const queryCallback_t callback);
  void unregisterQueryCallback(const std::string &id);

  void registerHandlerCallback(const std::string &id, const handlerCallback_t callback);
  void unregisterHandlerCallback(const std::string &id);

  void registerServer(rpcmanager_base::rpcserver_booter_base_sptr server);

  const std::string& type();

  const std::vector<std::string>& registeredServers();

private:
  template<class T, typename Tcallback>
  struct registerConfigureCallback_f: public std::unary_function<T,void>
  {
    registerConfigureCallback_f(const std::string &_id,  const Tcallback _callback)
      : id(_id), callback(_callback)
    {;}

    void operator()(T& x) { x->i()->registerConfigureCallback(id, callback); }
    const std::string& id;  const Tcallback& callback;
  };

  template<class T, typename Tcallback>
  struct unregisterConfigureCallback_f: public std::unary_function<T,void>
  {
    unregisterConfigureCallback_f(const std::string &_id)
      : id(_id)
    {;}

    void operator()(T& x) { x->i()->unregisterConfigureCallback(id); }
    const std::string&  id;
   };

  template<class T, typename Tcallback>
  struct registerQueryCallback_f: public std::unary_function<T,void>
  {
    registerQueryCallback_f(const std::string &_id,  const Tcallback _callback)
      : id(_id), callback(_callback)
    {;}

    void operator()(T& x) { x->i()->registerQueryCallback(id, callback); }
    const std::string& id;  const Tcallback& callback;
  };

  template<class T, typename Tcallback>
  struct unregisterQueryCallback_f: public std::unary_function<T,void>
  {
    unregisterQueryCallback_f(const std::string &_id)
      : id(_id)
    {;}

    void operator()(T& x) { x->i()->unregisterQueryCallback(id); }
    const std::string& id;
  };



  template<class T, typename Tcallback>
  struct registerHandlerCallback_f: public std::unary_function<T,void>
  {
    registerHandlerCallback_f(const std::string &_id,  const Tcallback _callback)
      : id(_id), callback(_callback)
    {;}

    void operator()(T& x) { x->i()->registerHandlerCallback(id, callback); }
    const std::string& id;  const Tcallback& callback;
  };

  template<class T, typename Tcallback>
  struct unregisterHandlerCallback_f: public std::unary_function<T,void>
  {
    unregisterHandlerCallback_f(const std::string &_id)
      : id(_id)
    {;}

    void operator()(T& x) { x->i()->unregisterHandlerCallback(id); }
    const std::string& id;
  };



  const std::string d_type;
  typedef std::vector<rpcmanager_base::rpcserver_booter_base_sptr> rpcServerMap_t;
  std::vector<std::string> d_registeredServers;
  rpcServerMap_t d_serverlist;
};

#endif /* RPCSERVER_AGGREGATOR_H */

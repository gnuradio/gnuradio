/* -*- c++ -*- */
/*
 * Copyright 2014,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "thrift/ControlPort.h"
#include <gnuradio/rpcserver_thrift.h>
#include <pmt/pmt.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>

#define DEBUG 0

using namespace rpcpmtconverter;

gr::logger_ptr rpcserver_thrift::d_logger;
gr::logger_ptr rpcserver_thrift::d_debug_logger;


rpcserver_thrift::rpcserver_thrift()
{
    gr::configure_default_loggers(d_logger, d_debug_logger, "rpcserver_thrift");
    // std::cerr << "rpcserver_thrift::ctor" ;
}

rpcserver_thrift::~rpcserver_thrift()
{
    // std::cerr << "rpcserver_thrift::dtor" ;
}

void rpcserver_thrift::registerConfigureCallback(const std::string& id,
                                                 const configureCallback_t callback)
{
    std::scoped_lock lock(d_callback_map_lock);
    {
        ConfigureCallbackMap_t::const_iterator iter(d_setcallbackmap.find(id));
        if (iter != d_setcallbackmap.end()) {
            std::stringstream s;
            s << "rpcserver_thrift:: rpcserver_thrift ERROR registering set, already "
                 "registered: "
              << id;
            throw std::runtime_error(s.str().c_str());
        }
    }

    if (DEBUG) {
        std::ostringstream msg;
        msg << "registering set: " << id;
        GR_LOG_INFO(d_debug_logger, msg.str());
    }
    d_setcallbackmap.insert(ConfigureCallbackMap_t::value_type(id, callback));
}

void rpcserver_thrift::unregisterConfigureCallback(const std::string& id)
{
    std::scoped_lock lock(d_callback_map_lock);
    ConfigureCallbackMap_t::iterator iter(d_setcallbackmap.find(id));
    if (iter == d_setcallbackmap.end()) {
        std::stringstream s;
        s << "rpcserver_thrift:: rpcserver_thrift ERROR unregistering set, not "
             "registered: "
          << id;
        throw std::runtime_error(s.str().c_str());
    }

    if (DEBUG) {
        std::ostringstream msg;
        msg << "unregistering set: " << id;
        GR_LOG_INFO(d_debug_logger, msg.str());
    }

    d_setcallbackmap.erase(iter);
}

void rpcserver_thrift::registerQueryCallback(const std::string& id,
                                             const queryCallback_t callback)
{
    std::scoped_lock lock(d_callback_map_lock);
    {
        QueryCallbackMap_t::const_iterator iter(d_getcallbackmap.find(id));
        if (iter != d_getcallbackmap.end()) {
            std::stringstream s;
            s << "rpcserver_thrift:: rpcserver_thrift ERROR registering get, already "
                 "registered: "
              << id;
            throw std::runtime_error(s.str().c_str());
        }
    }

    if (DEBUG) {
        std::ostringstream msg;
        msg << "registering get: " << id;
        GR_LOG_INFO(d_debug_logger, msg.str());
    }
    d_getcallbackmap.insert(QueryCallbackMap_t::value_type(id, callback));
}

void rpcserver_thrift::unregisterQueryCallback(const std::string& id)
{
    std::scoped_lock lock(d_callback_map_lock);
    QueryCallbackMap_t::iterator iter(d_getcallbackmap.find(id));
    if (iter == d_getcallbackmap.end()) {
        std::stringstream s;
        s << "rpcserver_thrift:: rpcserver_thrift ERROR unregistering get,  registered: "
          << id;
        throw std::runtime_error(s.str().c_str());
    }

    if (DEBUG) {
        std::ostringstream msg;
        msg << "unregistering get: " << id;
        GR_LOG_INFO(d_debug_logger, msg.str());
    }

    d_getcallbackmap.erase(iter);
}


void rpcserver_thrift::registerHandlerCallback(const std::string& id,
                                               const handlerCallback_t callback)
{
    std::scoped_lock lock(d_callback_map_lock);
    {
        HandlerCallbackMap_t::const_iterator iter(d_handlercallbackmap.find(id));
        if (iter != d_handlercallbackmap.end()) {
            std::stringstream s;
            s << "rpcserver_thrift:: rpcserver_thrift ERROR registering handler, already "
                 "registered: "
              << id;
            throw std::runtime_error(s.str().c_str());
        }
    }

    if (DEBUG) {
        std::ostringstream msg;
        msg << "registering handler: " << id;
        GR_LOG_INFO(d_debug_logger, msg.str());
    }
    d_handlercallbackmap.insert(HandlerCallbackMap_t::value_type(id, callback));
}

void rpcserver_thrift::unregisterHandlerCallback(const std::string& id)
{
    std::scoped_lock lock(d_callback_map_lock);
    HandlerCallbackMap_t::iterator iter(d_handlercallbackmap.find(id));
    if (iter == d_handlercallbackmap.end()) {
        std::stringstream s;
        s << "rpcserver_thrift:: rpcserver_thrift ERROR unregistering handler, "
             "registered: "
          << id;
        throw std::runtime_error(s.str().c_str());
    }

    if (DEBUG) {
        std::ostringstream msg;
        msg << "unregistering handler: " << id;
        GR_LOG_INFO(d_debug_logger, msg.str());
    }

    d_handlercallbackmap.erase(iter);
}


void rpcserver_thrift::setKnobs(const GNURadio::KnobMap& knobs)
{
    std::scoped_lock lock(d_callback_map_lock);
    std::for_each(knobs.begin(),
                  knobs.end(),
                  set_f<GNURadio::KnobMap::value_type, ConfigureCallbackMap_t>(
                      d_setcallbackmap, cur_priv));
}

void rpcserver_thrift::getKnobs(GNURadio::KnobMap& _return,
                                const GNURadio::KnobIDList& knobs)
{
    std::scoped_lock lock(d_callback_map_lock);
    if (knobs.empty()) {
        std::for_each(d_getcallbackmap.begin(),
                      d_getcallbackmap.end(),
                      get_all_f<QueryCallbackMap_t::value_type,
                                QueryCallbackMap_t,
                                GNURadio::KnobMap>(d_getcallbackmap, cur_priv, _return));
    } else {
        std::for_each(knobs.begin(),
                      knobs.end(),
                      get_f<GNURadio::KnobIDList::value_type, QueryCallbackMap_t>(
                          d_getcallbackmap, cur_priv, _return));
    }
}

void rpcserver_thrift::getRe(GNURadio::KnobMap& _return,
                             const GNURadio::KnobIDList& knobs)
{
    std::scoped_lock lock(d_callback_map_lock);
    if (knobs.empty()) {
        std::for_each(d_getcallbackmap.begin(),
                      d_getcallbackmap.end(),
                      get_all_f<QueryCallbackMap_t::value_type,
                                QueryCallbackMap_t,
                                GNURadio::KnobMap>(d_getcallbackmap, cur_priv, _return));
    } else {
        QueryCallbackMap_t::iterator it;
        for (it = d_getcallbackmap.begin(); it != d_getcallbackmap.end(); it++) {
            for (size_t j = 0; j < knobs.size(); j++) {
                const std::regex re(knobs[j]);
                if (std::regex_match(it->first, re)) {
                    get_f<GNURadio::KnobIDList::value_type, QueryCallbackMap_t>(
                        d_getcallbackmap, cur_priv, _return)(it->first);
                    break;
                }
            }
        }
    }
}

void rpcserver_thrift::properties(GNURadio::KnobPropMap& _return,
                                  const GNURadio::KnobIDList& knobs)
{
    std::scoped_lock lock(d_callback_map_lock);
    if (knobs.empty()) {
        std::for_each(
            d_getcallbackmap.begin(),
            d_getcallbackmap.end(),
            properties_all_f<QueryCallbackMap_t::value_type,
                             QueryCallbackMap_t,
                             GNURadio::KnobPropMap>(d_getcallbackmap, cur_priv, _return));
    } else {
        std::for_each(
            knobs.begin(),
            knobs.end(),
            properties_f<GNURadio::KnobIDList::value_type,
                         QueryCallbackMap_t,
                         GNURadio::KnobPropMap>(d_getcallbackmap, cur_priv, _return));
    }
}


void rpcserver_thrift::postMessage(const std::string& alias,
                                   const std::string& port,
                                   const std::string& msg)
{
    // alias and port are received as serialized PMT strings and need to
    // be deserialized into PMTs and then the actual info from there.
    // The actual message (msg) is also received as a serialized PMT. We
    // just need to get the PMT itself out of this to pass to the set_h
    // function for handling the message post.

    std::scoped_lock lock(d_callback_map_lock);

    pmt::pmt_t alias_pmt = pmt::deserialize_str(alias);
    pmt::pmt_t port_pmt = pmt::deserialize_str(port);
    pmt::pmt_t msg_pmt = pmt::deserialize_str(msg);
    std::string alias_str = pmt::symbol_to_string(alias_pmt);
    std::string port_str = pmt::symbol_to_string(port_pmt);
    std::string iface = alias_str + "::" + port_str;

    HandlerCallbackMap_t::iterator itr = d_handlercallbackmap.begin();
    for (; itr != d_handlercallbackmap.end(); itr++) {
        if (iface == (*itr).first) {
            set_h((*itr).second, cur_priv, port_pmt, msg_pmt);
        }
    }
}

void rpcserver_thrift::shutdown()
{
    if (DEBUG) {
        std::ostringstream msg;
        msg << "shutting down rpcserver_thrift... ";
        GR_LOG_INFO(d_debug_logger, msg.str());
    }
}

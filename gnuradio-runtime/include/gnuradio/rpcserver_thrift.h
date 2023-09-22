/* -*- c++ -*- */
/*
 * Copyright 2014,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef RPCSERVER_THRIFT_H
#define RPCSERVER_THRIFT_H

#include "thrift/ControlPort.h"
#include "thrift/gnuradio_types.h"
#include <gnuradio/logger.h>
#include <gnuradio/rpcpmtconverters_thrift.h>
#include <gnuradio/rpcserver_base.h>
#include <functional>
#include <map>
#include <mutex>
#include <string>

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)

class rpcserver_thrift : public virtual rpcserver_base, public GNURadio::ControlPortIf
{
public:
    rpcserver_thrift();
    virtual ~rpcserver_thrift();

    void registerConfigureCallback(const std::string& id,
                                   const configureCallback_t callback);
    void unregisterConfigureCallback(const std::string& id);

    void registerQueryCallback(const std::string& id, const queryCallback_t callback);
    void unregisterQueryCallback(const std::string& id);

    void registerHandlerCallback(const std::string& id, const handlerCallback_t callback);
    void unregisterHandlerCallback(const std::string& id);

    void setKnobs(const GNURadio::KnobMap&);
    void getKnobs(GNURadio::KnobMap&, const GNURadio::KnobIDList&);
    void getRe(GNURadio::KnobMap&, const GNURadio::KnobIDList&);
    void properties(GNURadio::KnobPropMap&, const GNURadio::KnobIDList& knobs);

    /*!
     *  \brief Call this to post a message to the \p port for the block
     *  identified by \p alias.
     *
     *  The message, \p msg, is passed as a serialized PMT that is then
     *  passed to the message handler function identified by \p port to
     *  the block identified by \p alias. The \p alias and \p port
     *  values are passed as serialized PMT symbols (see
     *  pmt::intern). The message is whatever PMT format is appropriate
     *  for the message handler function.
     *
     *  To use this function, the message handler function must have
     *  been registered (most likely in setup_rpc) in the block during
     *  construction using rpcbasic_register_handler.
     *
     *  \param alias The alias of the block, which is used to map to the
     *         real block through the global_block_registry. Passed in
     *         as a serialized PMT symbol.
     *  \param port The name of the message port. Passed in as a
     *         serialized PMT symbol.
     *  \param msg The actual message to pass to \p port. This is a
     *         serialized PMT where the PMT is whatever form appropriate
     *         for the message handler function.
     */
    void postMessage(const std::string& alias,
                     const std::string& port,
                     const std::string& msg);

    virtual void shutdown();

private:
    static gr::logger_ptr d_logger;
    static gr::logger_ptr d_debug_logger;

    std::mutex d_callback_map_lock;

    typedef std::map<std::string, configureCallback_t> ConfigureCallbackMap_t;
    ConfigureCallbackMap_t d_setcallbackmap;

    typedef std::map<std::string, queryCallback_t> QueryCallbackMap_t;
    QueryCallbackMap_t d_getcallbackmap;

    typedef std::map<std::string, handlerCallback_t> HandlerCallbackMap_t;
    HandlerCallbackMap_t d_handlercallbackmap;

    /*!
     * \brief Manages calling the callback function for a message handler posting.
     */
    void set_h(const handlerCallback_t& _handlerCallback,
               const priv_lvl_t& _cur_priv,
               pmt::pmt_t port,
               pmt::pmt_t msg)
    {
        if (cur_priv <= _handlerCallback.priv) {
            _handlerCallback.callback->post(port, msg);
        } else {
            std::ostringstream msg;
            msg << _handlerCallback.description
                << " requires PRIVLVL <= " << _handlerCallback.priv
                << " to set, currently at: " << cur_priv;
            GR_LOG_ERROR(d_logger, msg.str());
        }
    }


    template <typename T, typename TMap>
    struct set_f : public std::function<void(T)> {
        set_f(TMap& _setcallbackmap, const priv_lvl_t& _cur_priv)
            : d_setcallbackmap(_setcallbackmap), cur_priv(_cur_priv)
        {
            ;
        }

        void operator()(const T& p)
        {
            ConfigureCallbackMap_t::const_iterator iter(d_setcallbackmap.find(p.first));
            if (iter != d_setcallbackmap.end()) {
                if (cur_priv <= iter->second.priv) {
                    (*iter->second.callback)
                        .post(pmt::PMT_NIL, rpcpmtconverter::To_PMT::instance(p.second));
                } else {
                    std::ostringstream msg;
                    msg << "Key " << p.first
                        << " requires PRIVLVL <= " << iter->second.priv
                        << " to set, currently at: " << cur_priv;
                    GR_LOG_ERROR(d_logger, msg.str());
                }
            } else {
                throw apache::thrift::TApplicationException(__FILE__ " " S__LINE__);
            }
        }

        TMap& d_setcallbackmap;
        const priv_lvl_t& cur_priv;
    };

    template <typename T, typename TMap>
    struct get_f : public std::function<void(T)> {
        get_f(TMap& _getcallbackmap,
              const priv_lvl_t& _cur_priv,
              GNURadio::KnobMap& _outknobs)
            : d_getcallbackmap(_getcallbackmap), cur_priv(_cur_priv), outknobs(_outknobs)
        {
        }

        void operator()(const T& p)
        {
            QueryCallbackMap_t::const_iterator iter(d_getcallbackmap.find(p));
            if (iter != d_getcallbackmap.end()) {
                if (cur_priv <= iter->second.priv) {
                    outknobs[p] =
                        rpcpmtconverter::from_pmt((*iter->second.callback).retrieve());
                } else {
                    std::ostringstream msg;
                    msg << "Key " << iter->first
                        << " requires PRIVLVL: <= " << iter->second.priv
                        << " to get, currently at: " << cur_priv;
                    GR_LOG_ERROR(d_logger, msg.str());
                }
            } else {
                std::ostringstream smsgs;
                smsgs << "Ctrlport Key called with unregistered key (" << p << ")\n";
                GR_LOG_ERROR(d_logger, smsgs.str());
                throw apache::thrift::TApplicationException(__FILE__ " " S__LINE__);
            }
        }

        TMap& d_getcallbackmap;
        const priv_lvl_t& cur_priv;
        GNURadio::KnobMap& outknobs;
    };

    template <typename T, typename TMap, typename TKnobMap>
    struct get_all_f : public std::function<void(T)> {
        get_all_f(TMap& _getcallbackmap, const priv_lvl_t& _cur_priv, TKnobMap& _outknobs)
            : d_getcallbackmap(_getcallbackmap), cur_priv(_cur_priv), outknobs(_outknobs)
        {
            ;
        }

        void operator()(const T& p)
        {
            if (cur_priv <= p.second.priv) {
                outknobs[p.first] =
                    rpcpmtconverter::from_pmt(p.second.callback->retrieve());
            } else {
                std::ostringstream msg;
                msg << "Key " << p.first << " requires PRIVLVL: <= " << p.second.priv
                    << " to get, currently at: " << cur_priv;
                GR_LOG_ERROR(d_logger, msg.str());
            }
        }

        TMap& d_getcallbackmap;
        const priv_lvl_t& cur_priv;
        TKnobMap& outknobs;
    };

    template <typename T, typename TMap, typename TKnobMap>
    struct properties_all_f : public std::function<void(T)> {
        properties_all_f(QueryCallbackMap_t& _getcallbackmap,
                         const priv_lvl_t& _cur_priv,
                         GNURadio::KnobPropMap& _outknobs)
            : d_getcallbackmap(_getcallbackmap), cur_priv(_cur_priv), outknobs(_outknobs)
        {
            ;
        }

        void operator()(const T& p)
        {
            if (cur_priv <= p.second.priv) {
                GNURadio::KnobProp prop;
                prop.type = GNURadio::KnobType::KNOBDOUBLE;
                prop.units = p.second.units;
                prop.description = p.second.description;
                prop.min = rpcpmtconverter::from_pmt(p.second.min);
                prop.max = rpcpmtconverter::from_pmt(p.second.max);
                prop.display = static_cast<uint32_t>(p.second.display);
                outknobs[p.first] = prop;
            } else {
                std::ostringstream msg;
                msg << "Key " << p.first << " requires PRIVLVL: <= " << p.second.priv
                    << " to get, currently at: " << cur_priv;
                GR_LOG_ERROR(d_logger, msg.str());
            }
        }

        TMap& d_getcallbackmap;
        const priv_lvl_t& cur_priv;
        TKnobMap& outknobs;
    };

    template <class T, typename TMap, typename TKnobMap>
    struct properties_f : public std::function<void(T)> {
        properties_f(TMap& _getcallbackmap,
                     const priv_lvl_t& _cur_priv,
                     TKnobMap& _outknobs)
            : d_getcallbackmap(_getcallbackmap), cur_priv(_cur_priv), outknobs(_outknobs)
        {
            ;
        }

        void operator()(const T& p)
        {
            typename TMap::const_iterator iter(d_getcallbackmap.find(p));
            if (iter != d_getcallbackmap.end()) {
                if (cur_priv <= iter->second.priv) {
                    GNURadio::KnobProp prop;
                    prop.type = GNURadio::KnobType::KNOBDOUBLE;
                    prop.units = iter->second.units;
                    prop.description = iter->second.description;
                    prop.min = rpcpmtconverter::from_pmt(iter->second.min);
                    prop.max = rpcpmtconverter::from_pmt(iter->second.max);
                    prop.display = static_cast<uint32_t>(iter->second.display);
                    outknobs[p] = prop;
                } else {
                    std::ostringstream msg;
                    msg << "Key " << iter->first
                        << " requires PRIVLVL: <= " << iter->second.priv
                        << " to get, currently at: " << cur_priv;
                    GR_LOG_ERROR(d_logger, msg.str());
                }
            } else {
                throw apache::thrift::TApplicationException(__FILE__ " " S__LINE__);
            }
        }

        TMap& d_getcallbackmap;
        const priv_lvl_t& cur_priv;
        TKnobMap& outknobs;
    };
};

#endif /* RPCSERVER_THRIFT_H */

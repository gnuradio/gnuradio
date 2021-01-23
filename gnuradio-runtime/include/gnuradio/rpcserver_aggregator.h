/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef RPCSERVER_AGGREGATOR_H
#define RPCSERVER_AGGREGATOR_H

#include <gnuradio/rpcmanager_base.h>
#include <gnuradio/rpcserver_base.h>
#include <functional>
#include <string>
#include <vector>

class rpcserver_aggregator : public virtual rpcserver_base
{
public:
    rpcserver_aggregator();
    ~rpcserver_aggregator() override;

    void registerConfigureCallback(const std::string& id,
                                   const configureCallback_t callback) override;
    void unregisterConfigureCallback(const std::string& id) override;

    void registerQueryCallback(const std::string& id,
                               const queryCallback_t callback) override;
    void unregisterQueryCallback(const std::string& id) override;

    void registerHandlerCallback(const std::string& id,
                                 const handlerCallback_t callback) override;
    void unregisterHandlerCallback(const std::string& id) override;

    void registerServer(rpcmanager_base::rpcserver_booter_base_sptr server);

    const std::string& type();

    const std::vector<std::string>& registeredServers();

private:
    template <class T, typename Tcallback>
    struct registerConfigureCallback_f : public std::function<void(T)> {
        registerConfigureCallback_f(const std::string& _id, const Tcallback _callback)
            : id(_id), callback(_callback)
        {
            ;
        }

        void operator()(T& x) { x->i()->registerConfigureCallback(id, callback); }
        const std::string id;
        const Tcallback callback;
    };

    template <class T, typename Tcallback>
    struct unregisterConfigureCallback_f : public std::function<void(T)> {
        unregisterConfigureCallback_f(const std::string& _id) : id(_id) { ; }

        void operator()(T& x) { x->i()->unregisterConfigureCallback(id); }
        const std::string id;
    };

    template <class T, typename Tcallback>
    struct registerQueryCallback_f : public std::function<void(T)> {
        registerQueryCallback_f(const std::string& _id, const Tcallback _callback)
            : id(_id), callback(_callback)
        {
            ;
        }

        void operator()(T& x) { x->i()->registerQueryCallback(id, callback); }
        const std::string id;
        const Tcallback callback;
    };

    template <class T, typename Tcallback>
    struct unregisterQueryCallback_f : public std::function<void(T)> {
        unregisterQueryCallback_f(const std::string& _id) : id(_id) { ; }

        void operator()(T& x) { x->i()->unregisterQueryCallback(id); }
        const std::string id;
    };


    template <class T, typename Tcallback>
    struct registerHandlerCallback_f : public std::function<void(T)> {
        registerHandlerCallback_f(const std::string& _id, const Tcallback _callback)
            : id(_id), callback(_callback)
        {
            ;
        }

        void operator()(T& x) { x->i()->registerHandlerCallback(id, callback); }
        const std::string id;
        const Tcallback callback;
    };

    template <class T, typename Tcallback>
    struct unregisterHandlerCallback_f : public std::function<void(T)> {
        unregisterHandlerCallback_f(const std::string& _id) : id(_id) { ; }

        void operator()(T& x) { x->i()->unregisterHandlerCallback(id); }
        const std::string id;
    };


    const std::string d_type;
    typedef std::vector<rpcmanager_base::rpcserver_booter_base_sptr> rpcServerMap_t;
    std::vector<std::string> d_registeredServers;
    rpcServerMap_t d_serverlist;
};

#endif /* RPCSERVER_AGGREGATOR_H */

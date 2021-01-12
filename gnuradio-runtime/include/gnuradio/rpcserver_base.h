/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef RPCSERVER_BASE_H
#define RPCSERVER_BASE_H

#include <gnuradio/rpccallbackregister_base.h>

class rpcserver_base : public virtual callbackregister_base
{
public:
    rpcserver_base() : cur_priv(RPC_PRIVLVL_ALL) { ; }
    ~rpcserver_base() override { ; }

    void registerConfigureCallback(const std::string& id,
                                   const configureCallback_t callback) override = 0;
    void unregisterConfigureCallback(const std::string& id) override = 0;

    void registerQueryCallback(const std::string& id,
                               const queryCallback_t callback) override = 0;
    void unregisterQueryCallback(const std::string& id) override = 0;

    void registerHandlerCallback(const std::string& id,
                                 const handlerCallback_t callback) override = 0;
    void unregisterHandlerCallback(const std::string& id) override = 0;

    virtual void setCurPrivLevel(const priv_lvl_t priv) { cur_priv = priv; }

    typedef std::shared_ptr<rpcserver_base> rpcserver_base_sptr;

protected:
    priv_lvl_t cur_priv;

private:
};

#endif /* RPCSERVER_BASE_H */

/* -*- c++ -*- */
/*
 * Copyright 2008, 2009 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <usrp2/usrp2.h>
#include <boost/shared_ptr.hpp>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <gruel/thread.h>
#include <boost/foreach.hpp>

/*!
 * A properties accumulator allows finders to register found props.
 * Registering and getting the props is a thread safe operation.
 */
class props_accum{
private:
    usrp2::props_vector_t    d_found;
    gruel::mutex             d_mutex;
public:
    void operator()(const usrp2::props &p){
        gruel::scoped_lock l(d_mutex);
        d_found.push_back(p);
    }
    usrp2::props_vector_t get(void){
        gruel::scoped_lock l(d_mutex);
        return d_found;
    }
};

/*!
 * The base class for all finders.
 * TODO: expanded this base class to house common stuff (when more finders are implemented).
 */
class finder{
    public: virtual ~finder(){}
};

#include "eth_ctrl_transport.h"
/*!
 * The ethernet finder for locating usrps over raw ethernet.
 * The hint parameter must specify the name of an interface.
 */
class eth_finder : private usrp2::data_handler, public finder{
private:
    props_accum              *d_props_accum;
    usrp2::props             d_hint;
    usrp2::transport::sptr   d_ctrl_transport;
    boost::thread            *d_ctrl_thread;

public:
    eth_finder(props_accum *pa, const usrp2::props &hint): d_props_accum(pa), d_hint(hint){
        d_ctrl_transport = usrp2::transport::sptr(new usrp2::eth_ctrl_transport(d_hint.eth_args.ifc, d_hint.eth_args.mac_addr));
        //build the control packet
        op_generic_t op_id;
        memset(&op_id, 0, sizeof(op_id));
        op_id.opcode = OP_ID;
        op_id.len = sizeof(op_generic_t);
        //send the control packet
        iovec iov;
        iov.iov_base = &op_id;
        iov.iov_len = sizeof(op_generic_t);
        d_ctrl_transport->sendv(&iov, 1);
        //spawn new thread
        d_ctrl_thread = new boost::thread(boost::bind(&eth_finder::ctrl_thread_loop, this));
    }

    ~eth_finder(void){
        d_ctrl_thread->interrupt();
        d_ctrl_thread->join();
    }

    data_handler::result operator()(const void *base, size_t len){
        //copy the packet into an reply structure
        op_id_reply_t op_id_reply;
        memset(&op_id_reply, 0, sizeof(op_id_reply_t));
        memcpy(&op_id_reply, base, std::min(sizeof(op_id_reply_t), len));

        //inspect the reply packet and store into result
        if (op_id_reply.opcode != OP_ID_REPLY) // ignore
            return data_handler::DONE;
        usrp2::props p(usrp2::USRP_TYPE_ETH);
        p.eth_args.ifc = d_hint.eth_args.ifc;
        memcpy(&p.eth_args.mac_addr, &op_id_reply.addr, sizeof(p.eth_args.mac_addr));
        (*d_props_accum)(p);
        return data_handler::DONE;
    }

    void ctrl_thread_loop(void){
        while(true){
            d_ctrl_transport->recv(this);
            boost::this_thread::interruption_point();
        }
    }
};

namespace usrp2{

    props_vector_t
    find(const props &hint)
    {
        props_accum pa;
        std::vector <finder*> finders;
        //create finders based on the hint
        switch (hint.type){
        case USRP_TYPE_AUTO:
            finders.push_back(new eth_finder(&pa, hint));
            break;
        case USRP_TYPE_VIRTUAL: break;
        case USRP_TYPE_USB: break;
        case USRP_TYPE_ETH:
            finders.push_back(new eth_finder(&pa, hint));
            break;
        case USRP_TYPE_UDP: break;
        case USRP_TYPE_GPMC: break;
        }
        //allow responses to gather
        boost::this_thread::sleep(gruel::delta_time(0.05)); //50ms
        //destroy all the finders
        BOOST_FOREACH(finder *f, finders){delete f;}
        return pa.get();
    }

} // namespace usrp2


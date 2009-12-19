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

#include <usrp2_eth_packet.h>
#include <usrp2/usrp2.h>
#include <boost/shared_ptr.hpp>
#include <string.h>
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <gruel/thread.h>
#include <gruel/inet.h>
#include "eth_ctrl_transport.h"

#define FIND_DEBUG false

static const u2_mac_addr_t broadcast_mac_addr =
      {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }};

namespace usrp2{
    class find_helper : private data_handler{
    private:
        const std::string d_target_addr;
        transport::sptr   d_ctrl_transport;
        props_vector_t    d_result;

    public:
        typedef boost::shared_ptr<find_helper> sptr;

        find_helper(const std::string &ifc, const std::string &addr): d_target_addr(addr){
            d_ctrl_transport = transport::sptr(new eth_ctrl_transport(ifc, broadcast_mac_addr, false));
        }

        ~find_helper(void){/*NOP*/}

        props_vector_t find(void){
            //build the control packet
            op_generic_t op_id;
            op_id.opcode = OP_ID;
            op_id.len = sizeof(op_generic_t);
            //send the control packet
            iovec iov;
            iov.iov_base = &op_id;
            iov.iov_len = sizeof(op_generic_t);
            d_ctrl_transport->sendv(&iov, 1);
            //allow responses to gather
            boost::thread *ctrl_thread = new boost::thread(boost::bind(&find_helper::ctrl_thread_loop, this));
            boost::this_thread::sleep(gruel::delta_time(0.05)); //50ms
            ctrl_thread->interrupt();
            ctrl_thread->join();
            return d_result;
        }

        data_handler::result operator()(const void *base, size_t len){
            //copy the packet into an reply structure
            op_id_reply_t op_id_reply;
            memset(&op_id_reply, 0, sizeof(op_id_reply_t));
            memcpy(&op_id_reply, base, std::min(sizeof(op_id_reply_t), len));

            //inspect the reply packet and store into result
            if (op_id_reply.opcode != OP_ID_REPLY) // ignore
                return data_handler::DONE;
            props p = reply_to_props(&op_id_reply);
            if (FIND_DEBUG)
                std::cerr << "usrp2::find: response from " << p.addr << std::endl;
            if ((d_target_addr == "") || (d_target_addr == p.addr))
                d_result.push_back(p);
            return data_handler::DONE;
        }

        void ctrl_thread_loop(void){
            while(true){
                d_ctrl_transport->recv(this);
                boost::this_thread::interruption_point();
            }
        }

    private:
        static props
        reply_to_props(const op_id_reply_t *r)
        {
            const uint8_t *mac = (const uint8_t *)&r->addr;
            char addr_buf[128];
            snprintf(addr_buf, sizeof(addr_buf), "%02x:%02x:%02x:%02x:%02x:%02x",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
              
            props p;
            p.addr = std::string(addr_buf);  
            p.hw_rev = ntohs(r->hw_rev);
            memcpy(p.fpga_md5sum, r->fpga_md5sum, sizeof(p.fpga_md5sum));
            memcpy(p.sw_md5sum, r->sw_md5sum, sizeof(p.sw_md5sum));
            return p;
        }
    };

    props_vector_t
    find(const std::string &ifc, const std::string &addr)
    {
        if (FIND_DEBUG) {
            std::cerr << "usrp2::find: Searching interface " << ifc << " for "
                      << (addr == "" ? "all USRP2s" : addr)
                      << std::endl;
        }
        find_helper::sptr fh(new find_helper(ifc, addr));
        return fh->find();
    }

} // namespace usrp2


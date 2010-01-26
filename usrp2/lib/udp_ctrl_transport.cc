/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#include "udp_ctrl_transport.h"
#include <iostream>
#include <gruel/thread.h>

using boost::asio::ip::udp;

usrp2::udp_ctrl_transport::udp_ctrl_transport(const std::string &addr)
 : transport("udp control"){

    // resolve the address
    udp::resolver resolver(io_service_);
    udp::resolver::query query(udp::v4(), addr, "12345");
    receiver_endpoint_ = *resolver.resolve(query);
    
    // Create and open the socket
    socket_ = new udp::socket(io_service_);
    socket_->open(udp::v4());
 
    // Allow broadcasting
    boost::asio::socket_base::broadcast option(true);
    socket_->set_option(option);
}

usrp2::udp_ctrl_transport::~udp_ctrl_transport(void){
    delete socket_;
}

bool usrp2::udp_ctrl_transport::sendv(const iovec *iov, size_t iovlen){
    std::vector<boost::asio::const_buffer> buffs;
    for (size_t i = 0; i < iovlen; i++){
        buffs.push_back(
            boost::asio::buffer(iov[i].iov_base, iov[i].iov_len)
        );
    }
    socket_->send_to(buffs, receiver_endpoint_);
    return true;
}

void usrp2::udp_ctrl_transport::recv(data_handler *handler){
    // make sure that bytes are available
    if (not socket_->available()){
        boost::this_thread::sleep(gruel::delta_time(0.001)); // 1 ms
        return;
    }

    // receive the bytes and call the handler
    udp::endpoint sender_endpoint;
    size_t len = socket_->receive_from(
        boost::asio::buffer(_recv_buff, _recv_buff_len), sender_endpoint
    );
    (*handler)(_recv_buff, len);
}

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

#ifndef INCLUDED_UDP_CTRL_TRANSPORT_H
#define INCLUDED_UDP_CTRL_TRANSPORT_H

#include "transport.h"
#include <boost/asio.hpp>

namespace usrp2{

    class udp_ctrl_transport: public transport{
    private:
        boost::asio::ip::udp::socket *socket_;
        boost::asio::ip::udp::endpoint receiver_endpoint_;
        boost::asio::io_service io_service_;
        static const size_t _recv_buff_len = 1500;
        uint32_t _recv_buff[_recv_buff_len/sizeof(uint32_t)];

    public:
        /*!
         * \brief Create a new transport for the udp control
         * \param addr the address
         */
        udp_ctrl_transport(const std::string &addr, const std::string &port = "32768");
        ~udp_ctrl_transport();
        bool sendv(const iovec *iov, size_t iovlen);
        void recv(data_handler *handler);
        size_t max_bytes(void){
            return 1500 - 64; //size of headers FIXME magic numbers!!!
        }
};


} // namespace usrp2

#endif /* INCLUDED_UDP_CTRL_TRANSPORT_H */

/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

#ifndef INCLUDED_PROPS_H
#define INCLUDED_PROPS_H

#include <vector>
#include <string>
#include <iostream>
#include <gruel/inet.h>
#include <netinet/ether.h>

namespace usrp2{

  /*!
   * Wrapper for an ethernet mac address.
   * Provides conversion between string and binary formats.
   */
  struct u2_mac_addr{
    struct ether_addr d_mac_addr;
    u2_mac_addr(const std::string &str = "00:00:00:00:00:00");
    std::string to_string(void) const;
  };

  /*!
   * Wrapper for an ipv4 address.
   * Provides conversion between string and binary formats.
   */
  struct u2_ip_addr{
    struct in_addr d_ip_addr;
    u2_ip_addr(const std::string &str = "0.0.0.0");
    std::string to_string(void) const;
  };

  /*!
   * Possible usrp hardware types.
   *
   */
  typedef enum{
    USRP_TYPE_AUTO,
    USRP_TYPE_ETH,
    USRP_TYPE_UDP
  } usrp_type_t;

  /*!
   * Structure to hold properties to identify a usrp device.
   *
   */
  struct props{
    usrp_type_t type;
    struct{
        std::string ifc;
        std::string mac_addr;
    } eth_args;
    struct{
        std::string addr;
    } udp_args;

    /*!
     * \brief Convert a usrp props into a string representation
     */
    static const std::string to_string(const props &x);

    /*!
     * \brief Default constructor to initialize the props struct
     */
    props(usrp_type_t _type=USRP_TYPE_AUTO);
  };

  typedef std::vector<props> props_vector_t;

};

std::ostream& operator<<(std::ostream &os, const usrp2::props &x);
std::ostream& operator<<(std::ostream &os, const usrp2::u2_mac_addr &x);
std::ostream& operator<<(std::ostream &os, const usrp2::u2_ip_addr &x);

#endif /* INCLUDED_PROPS_H */

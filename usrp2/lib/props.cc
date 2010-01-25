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

#include <usrp2/props.h>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <stdexcept>

//----------------------- u2 mac addr wrapper ------------------------//
usrp2::u2_mac_addr::u2_mac_addr(const std::string &str_){
    std::string str = (str_ == "")? "ff:ff:ff:ff:ff:ff" : str_; 
    //ether_aton_r(str.c_str(), &d_mac_addr);
    bool good = false;
    char p[6] = {0x00, 0x50, 0xC2, 0x85, 0x30, 0x00}; // Matt's IAB

    switch (str.size()){
    case 5:
      good = sscanf(str.c_str(), "%hhx:%hhx", &p[4], &p[5]) == 2;
      break;
    case 17:
      good = sscanf(str.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
            &p[0], &p[1], &p[2], &p[3], &p[4], &p[5]) == 6;
      break;
    }

    if (not good) throw std::runtime_error("Invalid mac address: " + str);
    memcpy(&d_mac_addr, p, sizeof(d_mac_addr));
}

std::string usrp2::u2_mac_addr::to_string(void) const{
    char addr_buf[128];
    //ether_ntoa_r(&d_mac_addr, addr_buf);
    const uint8_t *p = reinterpret_cast<const uint8_t *>(&d_mac_addr);
    sprintf(addr_buf, "%02x:%02x:%02x:%02x:%02x:%02x",
        p[0], p[1], p[2], p[3], p[4], p[5]);
    return std::string(addr_buf);
}

std::ostream& operator<<(std::ostream &os, const usrp2::u2_mac_addr &x){
    os << x.to_string();
    return os;
}

//----------------------- u2 ipv4 wrapper ----------------------------//
usrp2::u2_ip_addr::u2_ip_addr(const std::string &str_){
    std::string str = (str_ == "")? "255.255.255.255" : str_; 
    int ret = inet_pton(AF_INET, str.c_str(), &d_ip_addr);
    if (ret == 0) throw std::runtime_error("Invalid ip address: " + str);
}

std::string usrp2::u2_ip_addr::to_string(void) const{
    char addr_buf[128];
    inet_ntop(AF_INET, &d_ip_addr, addr_buf, INET_ADDRSTRLEN);
    return std::string(addr_buf);
}

std::ostream& operator<<(std::ostream &os, const usrp2::u2_ip_addr &x){
    os << x.to_string();
    return os;
}

//----------------------- usrp props wrapper -------------------------//
usrp2::props::props(usrp_type_t _type){
    type = _type;
    eth_args.ifc = "eth0";
    eth_args.mac_addr = "";
    udp_args.addr = "";
}

const std::string usrp2::props::to_string(const props &x){
    std::stringstream out;
    out << "USRP Type: ";
    switch(x.type){
    case USRP_TYPE_AUTO:
        out << "Automatic" << std::endl;
        break;
    case USRP_TYPE_ETH:
        out << "Raw Ethernet" << std::endl;
        out << "Interface: " << x.eth_args.ifc << std::endl;
        out << "MAC Addr: " << x.eth_args.mac_addr << std::endl;
        break;
    case USRP_TYPE_UDP:
        out << "UDP Socket" << std::endl;
        out << "IP Addr: " << x.udp_args.addr << std::endl;
        break;
    default:
        out << "Unknown" << std::endl;
    }
    out << std::endl;
    return out.str();
}

std::ostream& operator<<(std::ostream &os, const usrp2::props &x)
{
  os << usrp2::props::to_string(x);
  return os;
}

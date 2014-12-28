/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <gnuradio/io_signature.h>
#include <gnuradio/block.h>
#include <sstream>
#include <cstring>

#define GR_HEADER_MAGIC 0x5FF0
#define GR_HEADER_VERSION 0x01

namespace gr {
  namespace zeromq {

    std::string 
    gen_tag_header(uint64_t &offset, std::vector<gr::tag_t> &tags) {

      uint16_t header_magic = GR_HEADER_MAGIC;
      uint8_t  header_version = GR_HEADER_VERSION;

      std::stringstream ss;
      size_t ntags = tags.size();
      ss.write( reinterpret_cast< const char* >( &header_magic ), sizeof(uint16_t) );
      ss.write( reinterpret_cast< const char* >( &header_version ), sizeof(uint8_t) );
    
      ss.write( reinterpret_cast< const char* >( &offset ), sizeof(uint64_t) );  // offset
      ss.write( reinterpret_cast< const char* >( &ntags ), sizeof(size_t) );      // num tags
      std::stringbuf sb("");
      //std::cout << "TX TAGS: (offset="<<offset<<" ntags="<<ntags<<")\n";
      for(size_t i=0; i<tags.size(); i++){
        //std::cout << "TX TAG: (" << tags[i].offset << ", " << tags[i].key << ", " << tags[i].value << ", " << tags[i].srcid << ")\n";
        ss.write( reinterpret_cast< const char* >( &tags[i].offset ), sizeof(uint64_t) );   // offset
        sb.str("");
        pmt::serialize( tags[i].key, sb );                                           // key
        pmt::serialize( tags[i].value, sb );                                         // value
        pmt::serialize( tags[i].srcid, sb );                                         // srcid
        ss.write( sb.str().c_str() , sb.str().length() );
      }

      return ss.str();
    }

    std::string
    parse_tag_header(std::string &buf_in, uint64_t &offset_out, std::vector<gr::tag_t> &tags_out) {

      std::istringstream iss( buf_in );
      size_t   rcv_ntags;

      uint16_t header_magic;
      uint8_t header_version;

      iss.read( (char*)&header_magic, sizeof(uint16_t ) );
      iss.read( (char*)&header_version, sizeof(uint8_t ) );
      if(header_magic != GR_HEADER_MAGIC){
        throw std::runtime_error("gr header magic does not match!");
        }
      if(header_version != 1){
        throw std::runtime_error("gr header version too high!");
        }

      iss.read( (char*)&offset_out, sizeof(uint64_t ) );
      iss.read( (char*)&rcv_ntags,  sizeof(size_t   ) );
      //std::cout << "RX TAGS: (offset="<<offset_out<<" ntags="<<rcv_ntags<<")\n";
      int rd_offset = sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint64_t) + sizeof(size_t);
      std::stringbuf sb( iss.str().substr(rd_offset) );

      for(size_t i=0; i<rcv_ntags; i++){
        gr::tag_t newtag;       
        sb.sgetn( (char*) &(newtag.offset), sizeof(uint64_t) );
        newtag.key   = pmt::deserialize( sb );
        newtag.value = pmt::deserialize( sb );
        newtag.srcid = pmt::deserialize( sb );
        //std::cout << "RX TAG: (" << newtag.offset << ", " << newtag.key << ", " << newtag.value << ", " << newtag.srcid << ")\n";
        tags_out.push_back(newtag);
        iss.str(sb.str());
      }

      int ndata = sb.in_avail();
      return iss.str().substr(iss.str().size() - ndata);
    }
  } /* namespace zeromq */
} /* namespace gr */

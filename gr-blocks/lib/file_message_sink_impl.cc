/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "file_message_sink_impl.h"
#include <stdexcept>

namespace gr {
  namespace blocks {

    file_message_sink::sptr
    file_message_sink::make(const char* filename, const char* keys)
    {
      return gnuradio::get_initial_sptr
        (new file_message_sink_impl(filename, keys));
    }
    void file_message_sink_impl::write_str(std::string& str){
          // Required call in file_sink_base to update pointer
          do_update();
          if(!d_fp ){
            std::cout << "d_fp is null" << std::endl;
            return;
          }
          if( fputs(str.c_str(),d_fp) == 0 ){
            if( ferror(d_fp)) {
              std::stringstream s;
              s << "file_message_sink write failed w/error " << fileno(d_fp) << std::endl;
              throw std::runtime_error(s.str());
            }
          }
    }
    void
      file_message_sink_impl::process_msg(pmt::pmt_t msg){
        pmt::pmt_t blob,dict;
        //pmt::print(msg);
        // If pair, assume (dict,blob) is the tuple
        // Otherwise, just assume msg is a PMT dictionary
        if( pmt::is_pair(msg) ){
          dict = pmt::car(msg);
          blob = pmt::cdr(msg);
        }else{
          dict = msg;
        }
        if( pmt::is_dict(msg) ){
          pmt::pmt_t spre_start = pmt::dict_ref(dict,pmt::intern("spre_start"),pmt::PMT_NIL);
          pmt::pmt_t pkt_end = pmt::dict_ref(dict,pmt::intern("pkt_end"),pmt::PMT_NIL);
          // std::cout << "spre_start=" << pmt::write_string(spre_start) << ","
          //   << "pkt_end=" << pmt::write_string(pkt_end) << std::endl;
          std::string spre_start_s = pmt::write_string(spre_start);
          std::string pkt_end_s = pmt::write_string(pkt_end);
          std::string line(spre_start_s + "," + pkt_end_s + "\n");
          write_str(line);
        }
      }
    /*
     * The private constructor
     */
    file_message_sink_impl::file_message_sink_impl(const char* filename, const char* keys)
      : gr::block("file_message_sink",
              gr::io_signature::make(0,0,0),
              gr::io_signature::make(0,0,0)),
      file_sink_base(filename,false,false)
    {
      message_port_register_in(pmt::mp("in"));
      set_msg_handler(pmt::mp("in"), boost::bind(&file_message_sink_impl::process_msg, this, _1));
      std::string keys_s(keys);
      // Parse comma-delimited keys into vector
      boost::split(d_keys,keys_s,boost::is_any_of(","));
      std::string hdr;
      // Write file header
      for( std::vector<std::string>::iterator it = d_keys.begin(); it != d_keys.end(); ++it){
        hdr += *it;
        if( it < d_keys.end()-1){
          hdr += ",";
        }
      }
      hdr += '\n';
      do_update();
      write_str(hdr);
    }

    /*
     * Our virtual destructor.
     */
    file_message_sink_impl::~file_message_sink_impl()
    {
    }
  } /* namespace blocks */
} /* namespace gr */


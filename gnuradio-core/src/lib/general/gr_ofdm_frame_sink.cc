/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#include <gr_ofdm_frame_sink.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <stdexcept>

#define VERBOSE 0

inline void
gr_ofdm_frame_sink::enter_search()
{
  if (VERBOSE)
    fprintf(stderr, "@ enter_search\n");

  d_state = STATE_SYNC_SEARCH;

}
    
inline void
gr_ofdm_frame_sink::enter_have_sync()
{
  if (VERBOSE)
    fprintf(stderr, "@ enter_have_sync\n");

  d_state = STATE_HAVE_SYNC;

  // clear state of demapper
  d_byte_offset = 0;
  d_partial_byte = 0;

  d_header = 0;
  d_headerbytelen_cnt = 0;
}

inline void
gr_ofdm_frame_sink::enter_have_header()
{
  d_state = STATE_HAVE_HEADER;

  // header consists of two 16-bit shorts in network byte order
  // payload length is lower 12 bits
  // whitener offset is upper 4 bits
  d_packetlen = (d_header >> 16) & 0x0fff;
  d_packet_whitener_offset = (d_header >> 28) & 0x000f;
  d_packetlen_cnt = 0;

  if (VERBOSE)
    fprintf(stderr, "@ enter_have_header (payload_len = %d) (offset = %d)\n", 
	    d_packetlen, d_packet_whitener_offset);
}

unsigned char gr_ofdm_frame_sink::bpsk_slicer(gr_complex x)
{
  return (unsigned char)(x.real() > 0 ? 1 : 0);
}

unsigned char gr_ofdm_frame_sink::qpsk_slicer(gr_complex x)
{
  unsigned char i = (x.real() > 0 ? 1 : 0);
  unsigned char q = (x.imag() > 0 ? 1 : 0);

  return (q << 1) | i;
}

unsigned int gr_ofdm_frame_sink::bpsk_demapper(const gr_complex *in,
					       unsigned char *out)
{
  unsigned int i=0, bytes_produced=0;

  while(i < d_occupied_carriers) {

    while((d_byte_offset < 8) && (i < d_occupied_carriers)) {
      //fprintf(stderr, "%f+j%f\n", in[i].real(), in[i].imag()); 
      d_partial_byte |= bpsk_slicer(in[i++]) << (d_byte_offset++);
    }

    if(d_byte_offset == 8) {
      out[bytes_produced++] = d_partial_byte;
      d_byte_offset = 0;
      d_partial_byte = 0;
    }
  }

  return bytes_produced;
}

unsigned int gr_ofdm_frame_sink::qpsk_demapper(const gr_complex *in,
					       unsigned char *out)
{
  unsigned int i=0, bytes_produced=0;

  while(i < d_occupied_carriers) {
    
    while((d_byte_offset < 8) && (i < d_occupied_carriers)) {
      //fprintf(stderr, "%f+j%f\n", in[i].real(), in[i].imag()); 
      d_partial_byte |= qpsk_slicer(in[i++]) << (d_byte_offset);
      d_byte_offset += 2;
    }

    if(d_byte_offset == 8) {
      out[bytes_produced++] = d_partial_byte;
      d_byte_offset = 0;
      d_partial_byte = 0;
    }
  }

  return bytes_produced;
}

gr_ofdm_frame_sink_sptr
gr_make_ofdm_frame_sink(gr_msg_queue_sptr target_queue, unsigned int occupied_carriers,
			const std::string &mod)
{
  return gr_ofdm_frame_sink_sptr(new gr_ofdm_frame_sink(target_queue, occupied_carriers, mod));
}


gr_ofdm_frame_sink::gr_ofdm_frame_sink(gr_msg_queue_sptr target_queue, unsigned int occupied_carriers,
				       const std::string &mod)
  : gr_sync_block ("ofdm_frame_sink",
		   gr_make_io_signature2 (2, 2, sizeof(gr_complex)*occupied_carriers, sizeof(char)),
		   gr_make_io_signature (0, 0, 0)),
    d_target_queue(target_queue), d_occupied_carriers(occupied_carriers), 
    d_byte_offset(0), d_partial_byte(0)
{
  d_bytes_out = new unsigned char[(int)ceil(d_occupied_carriers/4.0)];

  if(mod == "qpsk") {
    d_demapper = &gr_ofdm_frame_sink::qpsk_demapper;
  }
  else if(mod == "bpsk") {
    d_demapper = &gr_ofdm_frame_sink::bpsk_demapper;
  }
  else {
    throw std::invalid_argument("Modulation type must be BPSK or QPSK.");  
  }

  enter_search();
}

gr_ofdm_frame_sink::~gr_ofdm_frame_sink ()
{
  delete [] d_bytes_out;
}

int
gr_ofdm_frame_sink::work (int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  const char *sig = (const char *) input_items[1];
  unsigned int j = 0;
  unsigned int bytes=0;
  
  if (VERBOSE)
    fprintf(stderr,">>> Entering state machine\n");
  
  //bytes = bpsk_demapper(&in[0], d_bytes_out);
  bytes = (this->*d_demapper)(&in[0], d_bytes_out);  

  switch(d_state) {
      
  case STATE_SYNC_SEARCH:    // Look for flag indicating beginning of pkt
    if (VERBOSE)
      fprintf(stderr,"SYNC Search, noutput=%d\n", noutput_items);
    
    if (sig[0]) {  // Found it, set up for header decode
      enter_have_sync();
    }
    break;

  case STATE_HAVE_SYNC:
    if (VERBOSE) {
      if(sig[0])
	printf("ERROR -- Found SYNC in HAVE_SYNC\n");
      fprintf(stderr,"Header Search bitcnt=%d, header=0x%08x\n",
	      d_headerbytelen_cnt, d_header);
    }

    j = 0;
    while(j < bytes) {
      d_header = (d_header << 8) | (d_bytes_out[j] & 0xFF);
      j++;
      
      if (++d_headerbytelen_cnt == HEADERBYTELEN) {
	
	if (VERBOSE)
	  fprintf(stderr, "got header: 0x%08x\n", d_header);
	
	// we have a full header, check to see if it has been received properly
	if (header_ok()){
	  enter_have_header();
	  
	  if (VERBOSE)
	    printf("\nPacket Length: %d\n", d_packetlen);
	  
	  while((j < bytes) && (d_packetlen_cnt < d_packetlen)) {
	    d_packet[d_packetlen_cnt++] = d_bytes_out[j++];
	  }
	  
	  if(d_packetlen_cnt == d_packetlen) {
	    gr_message_sptr msg =
	      gr_make_message(0, d_packet_whitener_offset, 0, d_packetlen);
	    memcpy(msg->msg(), d_packet, d_packetlen_cnt);
	    d_target_queue->insert_tail(msg);		// send it
	    msg.reset();  				// free it up
	    
	    enter_search();				
	  }
	}
	else {
	  enter_search();				// bad header
	}
      }
    }
    break;
      
  case STATE_HAVE_HEADER:
    if (VERBOSE) {
      if(sig[0])
	printf("ERROR -- Found SYNC in HAVE_HEADER at %d, length of %d\n", d_packetlen_cnt, d_packetlen);
      fprintf(stderr,"Packet Build\n");
    }
    
    j = 0;
    while(j < bytes) {
      d_packet[d_packetlen_cnt++] = d_bytes_out[j++];
      
      if (d_packetlen_cnt == d_packetlen){		// packet is filled
	// build a message
	// NOTE: passing header field as arg1 is not scalable
	gr_message_sptr msg =
	  gr_make_message(0, d_packet_whitener_offset, 0, d_packetlen_cnt);
	memcpy(msg->msg(), d_packet, d_packetlen_cnt);
	
	d_target_queue->insert_tail(msg);		// send it
	msg.reset();  				// free it up
	
	enter_search();
	break;
      }
    }
    break;
    
  default:
    assert(0);
    
  } // switch
  
  return 1;
}

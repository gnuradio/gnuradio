/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_PAGER_FLEX_PARSE_H
#define INCLUDED_PAGER_FLEX_PARSE_H

#include <pager_api.h>
#include <gr_sync_block.h>
#include <gr_msg_queue.h>
#include <pageri_flex_modes.h>
#include <sstream>

class pager_flex_parse;
typedef boost::shared_ptr<pager_flex_parse> pager_flex_parse_sptr;

PAGER_API pager_flex_parse_sptr pager_make_flex_parse(gr_msg_queue_sptr queue, float freq);

#define FIELD_DELIM ((unsigned char)128)

/*!
 * \brief flex parse description
 * \ingroup pager_blk
 */
class PAGER_API pager_flex_parse : public gr_sync_block
{
private:
    // Constructors
    friend PAGER_API pager_flex_parse_sptr pager_make_flex_parse(gr_msg_queue_sptr queue, float freq);
    pager_flex_parse(gr_msg_queue_sptr queue, float freq);

    std::ostringstream d_payload;
    gr_msg_queue_sptr d_queue;		  // Destination for decoded pages

    int d_count;	                  // Count of received codewords
    gr_int32 d_datawords[88];             // 11 blocks of 8 32-bit words

    page_type_t d_type;		  	  // Current page type
    int d_capcode;	                  // Current page destination address
    bool d_laddr;	                  // Current page has long address
    float d_freq;			  // Channel frequency
    
    void parse_data();	      		  // Handle a frame's worth of data
    void parse_capcode(gr_int32 aw1, gr_int32 aw2);     
    void parse_alphanumeric(int mw1, int mw2, int j);
    void parse_numeric(int mw1, int mw2, int j);
    void parse_tone_only();
    void parse_unknown(int mw1, int mw2);
    
public:
    int work(int noutput_items,
        gr_vector_const_void_star &input_items, 
        gr_vector_void_star &output_items);
};

#endif /* INCLUDED_PAGER_FLEX_PARSE_H */

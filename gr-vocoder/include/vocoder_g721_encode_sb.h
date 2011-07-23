/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOCODER_G721_ENCODE_SB_H
#define INCLUDED_VOCODER_G721_ENCODE_SB_H

#include <vocoder_api.h>
#include <gr_sync_block.h>

class vocoder_g721_encode_sb;

typedef boost::shared_ptr<vocoder_g721_encode_sb> vocoder_g721_encode_sb_sptr;

VOCODER_API vocoder_g721_encode_sb_sptr vocoder_make_g721_encode_sb();

/*!
 * \brief This block performs g721 audio encoding. 
 *
 * \ingroup vocoder_blk
 */

class VOCODER_API vocoder_g721_encode_sb : virtual public gr_sync_block
{
};

#endif /* INCLUDED_VOCODER_G721_ENCODE_SB_H */

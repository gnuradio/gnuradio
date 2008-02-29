/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#ifndef MSDD_BUFFER_COPY_BEHAVIORS_H_
#define MSDD_BUFFER_COPY_BEHAVIORS_H_

namespace msdd {
	
  class BufferCopyBehavior 
  {
  public: 
    virtual void operator() (gr_vector_void_star &a, const void * b, unsigned int output_index, unsigned int nitems) = 0;
    virtual ~BufferCopyBehavior() {};
  };
	  
  template <class Tin, class Tout> 
  class BufferCopyBehaviorGeneric : public BufferCopyBehavior {
    void operator() (gr_vector_void_star &a, const void * b, unsigned int output_index, unsigned int nitems) {
      Tout 	*out(&(reinterpret_cast<Tout *>(a[0]))[output_index]);	// sloppy
      const Tin 	*in(reinterpret_cast<const Tin *>(b)); // equisloppy
      
      for (unsigned int i = 0; i < nitems; ++i) {
	out[i] = in[i];
      }
    }
  };
  
  template <class Tin>
  class BufferCopyBehaviorComplex : public BufferCopyBehavior {
    void operator() (gr_vector_void_star &a, const void * b, unsigned int output_index, unsigned int nitems) {
      gr_complex 	*out(&(reinterpret_cast<gr_complex *>(a[0]))[output_index]);	// sloppy
      const Tin 	*in(reinterpret_cast<const Tin *>(b)); // equisloppy
      
      for (unsigned int i = 0; i < nitems; ++i) {
	out[i] = gr_complex (in[4*i+1],in[4*i+3]);
      }
    }
  };
}

#endif /*MSDD_BUFFER_COPY_BEHAVIORS_H_*/

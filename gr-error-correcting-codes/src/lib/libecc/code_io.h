/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#ifndef INCLUDED_CODE_IO_H
#define INCLUDED_CODE_IO_H

#include "code_types.h"
#include <vector>
#include <assert.h>
#include <iostream>

/*
 * code_io provides classes which do the input and output for these
 * codes.  One can add another class with its specific needs primarily
 * by changing the way that the provided data is manipulated to read
 * or write data items.
 */

/*
 * combined io classes
 */

class code_io
{
public:
  inline code_io (size_t n_streams) {
    if (n_streams < 1) {
      std::cerr << "code_io::code_io: Error:" <<
	"Provided # of streams (" << n_streams <<
	") must be at least 1.\n";
      assert (0);
    }
    d_n_streams = n_streams;
    d_buf_ptrs.resize (d_n_streams);
  };

  virtual ~code_io () {};

  inline void set_buffer (void** buffer, size_t n_items) {
    if (buffer == 0) {
      std::cerr << "code_io::set_buffer: Error:" <<
	"Provided buffer is NULL.\n";
      assert (0);
    }
    if (n_items == 0) {
      std::cerr << "code_io::set_buffer: Warning:" <<
	"Provided # of items is 0!\n";
    }
    d_buf = buffer;
    d_n_items = d_n_items_left = n_items;
    for (size_t m = 0; m < d_n_streams; m++) {
      d_buf_ptrs[m] = d_buf[m];
    }
  };

  inline void set_buffer (std::vector<void*>* buffer, size_t n_items) {
    set_buffer (&((*buffer)[0]), n_items);};

  inline void set_buffer (std::vector<void*>& buffer, size_t n_items) {
    set_buffer (&(buffer[0]), n_items);};

  inline virtual void increment_indices () {
    if (d_n_items_left == 0) {
      std::cerr << "code_io::increment_indices: Warning: "
	"No items left!\n";
    } else
      d_n_items_left--;
  };

  // methods for getting info on class internals

  const size_t& n_items_left () {return (d_n_items_left);};
  const size_t n_items_used () {return (d_n_items - d_n_items_left);};
  const size_t n_streams () {return (d_n_streams);};

protected:
  void** d_buf;
  std::vector<void*> d_buf_ptrs;
  size_t d_n_streams, d_n_items, d_n_items_left;
};

/*
 * input classes
 */

class code_input : public code_io
{
public:
  inline code_input (size_t n_streams) : code_io (n_streams) {};
  virtual ~code_input () {};
  virtual void read_item (void* item, size_t stream_n) = 0;
  virtual void read_items (void* items) = 0;
  inline virtual void increment_indices () {code_io::increment_indices ();};
};

typedef code_input* code_input_ptr;

class code_input_id : public code_input
{
private:
  typedef double input_t;

public:
/*!
 * class code_input_id : public code_input
 *
 * "id":
 *   'i': one stream per code input as defined by the instantiated
 *        code ("individual", not mux'ed);
 *   'f': streams of double;
 */
  inline code_input_id (size_t n_streams) : code_input (n_streams) {};
  virtual ~code_input_id () {};

  inline virtual void read_item (void* item, size_t stream_n) {
    /* no error checking for better speed! */
    input_t* t_item = (input_t*) item;
    (*t_item) = (*((input_t*)(d_buf_ptrs[stream_n])));
  };
  inline virtual void read_items (void* items) {
    /* no error checking for better speed! */
    input_t* t_items = (input_t*) items;
    for (size_t m = 0; m < d_n_streams; m++) {
      t_items[m] = (*((input_t*)(d_buf_ptrs[m])));
    }
  };
  inline virtual void increment_indices () {
    code_input::increment_indices ();
    for (size_t m = 0; m < d_n_streams; m++) {
      input_t* t_buf = (input_t*) d_buf_ptrs[m];
      d_buf_ptrs[m] = (void*)(++t_buf);
    }
  };
};

class code_input_if : public code_input
{
private:
  typedef float input_t;

public:
/*!
 * class code_input_if : public code_input
 *
 * "if":
 *   'i': one stream per code input as defined by the instantiated
 *        code ("individual", not mux'ed);
 *   'f': streams of float;
 */
  inline code_input_if (size_t n_streams) : code_input (n_streams) {};
  virtual ~code_input_if () {};

  inline virtual void read_item (void* item, size_t stream_n) {
    /* no error checking for better speed! */
    input_t* t_item = (input_t*) item;
    (*t_item) = (*((input_t*)(d_buf_ptrs[stream_n])));
  };
  inline virtual void read_items (void* items) {
    /* no error checking for better speed! */
    input_t* t_items = (input_t*) items;
    for (size_t m = 0; m < d_n_streams; m++) {
      t_items[m] = (*((input_t*)(d_buf_ptrs[m])));
    }
  };
  inline virtual void increment_indices () {
    code_input::increment_indices ();
    for (size_t m = 0; m < d_n_streams; m++) {
      input_t* t_buf = (input_t*) d_buf_ptrs[m];
      d_buf_ptrs[m] = (void*)(++t_buf);
    }
  };
};

class code_input_ic : public code_input
{
private:
  typedef char input_t;

public:
/*!
 * class code_input_is : public code_input
 *
 * "ic":
 *   'i': one stream per code input as defined by the instantiated
 *        code ("individual", not mux'ed);
 *   'c': streams of char;
 */
  inline code_input_ic (size_t n_streams) : code_input (n_streams) {};
  virtual ~code_input_ic () {};

  inline virtual void read_item (void* item, size_t stream_n) {
    /* no error checking for better speed! */
    input_t* t_item = (input_t*) item;
    (*t_item) = (*((input_t*)(d_buf_ptrs[stream_n])));
  };
  inline virtual void read_items (void* items) {
    /* no error checking for better speed! */
    input_t* t_items = (input_t*) items;
    for (size_t m = 0; m < d_n_streams; m++) {
      t_items[m] = (*((input_t*)(d_buf_ptrs[m])));
    }
  };
  inline virtual void increment_indices () {
    code_input::increment_indices ();
    for (size_t m = 0; m < d_n_streams; m++) {
      input_t* t_buf = (input_t*) d_buf_ptrs[m];
      d_buf_ptrs[m] = (void*)(++t_buf);
    }
  };
};

class code_input_is : public code_input
{
private:
  typedef short input_t;

public:
/*!
 * class code_input_is : public code_input
 *
 * "is":
 *   'i': one stream per code input as defined by the instantiated
 *        code ("individual", not mux'ed);
 *   's': streams of short;
 */
  inline code_input_is (size_t n_streams) : code_input (n_streams) {};
  virtual ~code_input_is () {};

  inline virtual void read_item (void* item, size_t stream_n) {
    /* no error checking for better speed! */
    input_t* t_item = (input_t*) item;
    (*t_item) = (*((input_t*)(d_buf_ptrs[stream_n])));
  };
  inline virtual void read_items (void* items) {
    /* no error checking for better speed! */
    input_t* t_items = (input_t*) items;
    for (size_t m = 0; m < d_n_streams; m++) {
      t_items[m] = (*((input_t*)(d_buf_ptrs[m])));
    }
  };
  inline virtual void increment_indices () {
    code_input::increment_indices ();
    for (size_t m = 0; m < d_n_streams; m++) {
      input_t* t_buf = (input_t*) d_buf_ptrs[m];
      d_buf_ptrs[m] = (void*)(++t_buf);
    }
  };
};

class code_input_il : public code_input
{
private:
  typedef long input_t;

public:
/*!
 * class code_input_il : public code_input
 *
 * "il":
 *   'i': one stream per code input as defined by the instantiated
 *        code ("individual", not mux'ed);
 *   'l': streams of long;
 */
  inline code_input_il (size_t n_streams) : code_input (n_streams) {};
  virtual ~code_input_il () {};

  inline virtual void read_item (void* item, size_t stream_n) {
    /* no error checking for better speed! */
    input_t* t_item = (input_t*) item;
    (*t_item) = (*((input_t*)(d_buf_ptrs[stream_n])));
  };
  inline virtual void read_items (void* items) {
    /* no error checking for better speed! */
    input_t* t_items = (input_t*) items;
    for (size_t m = 0; m < d_n_streams; m++) {
      t_items[m] = (*((input_t*)(d_buf_ptrs[m])));
    }
  };
  inline virtual void increment_indices () {
    code_input::increment_indices ();
    for (size_t m = 0; m < d_n_streams; m++) {
      input_t* t_buf = (input_t*) d_buf_ptrs[m];
      d_buf_ptrs[m] = (void*)(++t_buf);
    }
  };
};

class code_input_ill : public code_input
{
private:
  typedef long long input_t;

public:
/*!
 * class code_input_ill : public code_input
 *
 * "ill":
 *   'i': one stream per code input as defined by the instantiated
 *        code ("individual", not mux'ed);
 *   'll': streams of long long;
 */
  inline code_input_ill (size_t n_streams) : code_input (n_streams) {};
  virtual ~code_input_ill () {};

  inline virtual void read_item (void* item, size_t stream_n) {
    /* no error checking for better speed! */
    input_t* t_item = (input_t*) item;
    (*t_item) = (*((input_t*)(d_buf_ptrs[stream_n])));
  };
  inline virtual void read_items (void* items) {
    /* no error checking for better speed! */
    input_t* t_items = (input_t*) items;
    for (size_t m = 0; m < d_n_streams; m++) {
      t_items[m] = (*((input_t*)(d_buf_ptrs[m])));
    }
  };
  inline virtual void increment_indices () {
    code_input::increment_indices ();
    for (size_t m = 0; m < d_n_streams; m++) {
      input_t* t_buf = (input_t*) d_buf_ptrs[m];
      d_buf_ptrs[m] = (void*)(++t_buf);
    }
  };
};

class code_input_ic1 : public code_input
{
public:
/*!
 * class code_input_ic1 : public code_input
 *
 * "ic":
 *   'i': one stream per code input as defined by the instantiated
 *        code ("individual", not mux'ed);
 *   'c': streams of char;
 *   '1': single bit per char;
 * --> which bit to choose left to an inheriting class
 */

  inline code_input_ic1 (size_t n_streams) : code_input (n_streams) {};
  virtual ~code_input_ic1 () {};

  inline virtual void read_item (void* item, size_t stream_n) {
    /* no error checking for better speed! */
    char* t_item = (char*) item;
    (*t_item) = (*((char*)(d_buf_ptrs[stream_n]))) & d_which_bit;
  };
  inline virtual void read_items (void* items) {
    /* no error checking for better speed! */
    char* t_items = (char*) items;
    for (size_t m = 0; m < d_n_streams; m++) {
      t_items[m] = (*((char*)(d_buf_ptrs[m]))) & d_which_bit;
    }
  };
  inline virtual void increment_indices () {
    code_input::increment_indices ();
    for (size_t m = 0; m < d_n_streams; m++) {
      char* t_buf = (char*) d_buf_ptrs[m];
      d_buf_ptrs[m] = (void*)(++t_buf);
    }
  };

protected:
  char d_which_bit;
};

class code_input_ic1l : public code_input_ic1
{
public:
/*!
 * class code_input_ic1l : public code_input_ic1
 *
 * "ic1l":
 *   'i': one stream per code input as defined by the instantiated
 *        code ("individual", not mux'ed);
 *   'c': streams of char;
 *   '1': single bit per char;
 *   'l': using only the LSB of the char. 
 */

  inline code_input_ic1l (size_t n_streams) :
    code_input_ic1 (n_streams) {d_which_bit = 1;};
  virtual ~code_input_ic1l () {};
};

class code_input_ic1h : public code_input_ic1
{
public:
/*!
 * class code_input_ic1h : public code_input_ic1
 *
 * "ic1h":
 *   'i': one stream per code input as defined by the instantiated
 *        code ("individual", not mux'ed);
 *   'c': streams of char;
 *   '1': single bit per char;
 *   'h': using only the MSB of the char.
 */

  inline code_input_ic1h (size_t n_streams)
    : code_input_ic1 (n_streams) {d_which_bit = 128;};
  virtual ~code_input_ic1h () {};
};

class code_input_ic8l : public code_input_ic1l
{
protected:
  size_t d_bit_shift;
  const static size_t g_num_bits_per_byte = 8;

public:
/*!
 * class code_input_ic8l : public code_input_ic1l
 *
 * "ic8l":
 *   'i': one stream per code input as defined by the instantiated
 *        code ("individual", not mux'ed);
 *   'c': streams of char;
 *   '8': using all 8 bits per char;
 *   'l': starting with the LSB and working up
 */

  inline code_input_ic8l (size_t n_streams) :
    code_input_ic1l (n_streams) {d_bit_shift = 0;};

  virtual ~code_input_ic8l () {};

  inline virtual void increment_indices () {
    code_input::increment_indices ();
    if (++d_bit_shift % g_num_bits_per_byte == 0) {
      d_bit_shift = 0;
      for (size_t m = 0; m < d_n_streams; m++) {
	char* t_buf = (char*) d_buf_ptrs[m];
	d_buf_ptrs[m] = (void*)(++t_buf);
      }
    } else {
      for (size_t m = 0; m < d_n_streams; m++) {
	char* t_buf = (char*) d_buf_ptrs[m];
	(*t_buf) >>= 1;
      }
    }
  };
};

class code_input_ic8h : public code_input_ic1h
{
protected:
  size_t d_bit_shift;
  const static size_t g_num_bits_per_byte = 8;

public:
/*!
 * class code_input_ic8h : public code_input_ic1h
 *
 * "ic8h":
 *   'i': one stream per code input as defined by the instantiated
 *        code ("individual", not mux'ed);
 *   'c': streams of char;
 *   '8': using all 8 bits per char;
 *   'h': starting with the MSB and working down
 */

  inline code_input_ic8h (size_t n_streams) :
    code_input_ic1h (n_streams) {d_bit_shift = 0;};

  virtual ~code_input_ic8h () {};

  inline virtual void increment_indices () {
    code_input::increment_indices ();
    if (++d_bit_shift % g_num_bits_per_byte == 0) {
      d_bit_shift = 0;
      for (size_t m = 0; m < d_n_streams; m++) {
	char* t_buf = (char*) d_buf_ptrs[m];
	d_buf_ptrs[m] = (void*)(++t_buf);
      }
    } else {
      for (size_t m = 0; m < d_n_streams; m++) {
	char* t_buf = (char*) d_buf_ptrs[m];
	(*t_buf) <<= 1;
      }
    }
  };
};

/*
 * output classes
 */

class code_output : public code_io
{
public:
  code_output (size_t n_streams) : code_io (n_streams) {};
  virtual ~code_output () {};
  virtual void write_item (const void* item, size_t stream_n) = 0;
  virtual void write_items (const void* items) = 0;
  virtual inline void increment_indices () {code_io::increment_indices ();};
};

typedef code_output* code_output_ptr;

class code_output_ic1l : public code_output
{
public:
/*!
 * class code_output_ic1l : public code_output
 *
 *   'i': one stream per code input as defined by the instantiated
 *        code ("individual", not mux'ed);
 *   'c': streams of char;
 *   '1': single bit per char;
 *   'l': using only the right-most justified (LSB).
 */

  inline code_output_ic1l (size_t n_streams) : code_output (n_streams) {};
  virtual ~code_output_ic1l () {};

  inline virtual void write_item (const void* item, size_t stream_n) {
    /* no error checking for better speed! */
    const char* t_item = (char*) item;
    (*((char*)(d_buf_ptrs[stream_n]))) = (*t_item) & 1;
  };
  inline virtual void write_items (const void* items) {
    /* no error checking for better speed! */
    const char* t_items = (char*) items;
    for (size_t m = 0; m < d_n_streams; m++)
      (*((char*)(d_buf_ptrs[m]))) = (t_items[m]) & 1;
  };
  inline virtual void increment_indices () {
    code_output::increment_indices ();
    for (size_t m = 0; m < d_n_streams; m++) {
      char* t_buf = (char*) d_buf_ptrs[m];
      d_buf_ptrs[m] = (void*)(++t_buf);
    }
  };
};

#endif /* INCLUDED_CODE_IO_H */

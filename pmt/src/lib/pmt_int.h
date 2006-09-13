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
#ifndef INCLUDED_PMT_INT_H
#define INCLUDED_PMT_INT_H

#include <pmt.h>

/*
 * EVERYTHING IN THIS FILE IS PRIVATE TO THE IMPLEMENTATION!
 *
 * See pmt.h for the public interface
 */

class pmt_base {
protected:
  pmt_base(){};
  virtual ~pmt_base();

private:
  pmt_base(const pmt_base& rhs);		// NOT IMPLEMENTED
  pmt_base& operator=(const pmt_base& rhs);	// NOT IMPLEMENTED
  

public:
  virtual bool is_bool()    const { return false; }
  virtual bool is_symbol()  const { return false; }
  virtual bool is_number()  const { return false; }
  virtual bool is_integer() const { return false; }
  virtual bool is_real()    const { return false; }
  virtual bool is_complex() const { return false; }
  virtual bool is_null()    const { return false; }
  virtual bool is_pair()    const { return false; }
  virtual bool is_vector()  const { return false; }
  virtual bool is_dict()    const { return false; }

  virtual bool is_uniform_vector() const { return false; }
  virtual bool is_u8vector()  const { return false; }
  virtual bool is_s8vector()  const { return false; }
  virtual bool is_u16vector() const { return false; }
  virtual bool is_s16vector() const { return false; }
  virtual bool is_u32vector() const { return false; }
  virtual bool is_s32vector() const { return false; }
  virtual bool is_u64vector() const { return false; }
  virtual bool is_s64vector() const { return false; }
  virtual bool is_f32vector() const { return false; }
  virtual bool is_f64vector() const { return false; }
  virtual bool is_c32vector() const { return false; }
  virtual bool is_c64vector() const { return false; }

};

class pmt_bool : public pmt_base
{
public:
  pmt_bool();
  //~pmt_bool(){}

  bool is_bool() const { return true; }
};


class pmt_symbol : public pmt_base
{
  std::string	d_name;
  pmt_t		d_next;
  
public:
  pmt_symbol(const std::string &name);
  //~pmt_symbol(){}

  bool is_symbol() const { return true; }
  const std::string name() { return d_name; }

  pmt_t next() { return d_next; }		// symbol table link
  void set_next(pmt_t next) { d_next = next; }
};

class pmt_integer : public pmt_base
{
  long		d_value;

public:
  pmt_integer(long value);
  //~pmt_integer(){}

  bool is_integer() const { return true; }
  long value() const { return d_value; }
};

class pmt_real : public pmt_base
{
  double	d_value;

public:
  pmt_real(double value);
  //~pmt_real(){}

  bool is_real() const { return true; }
  double value() const { return d_value; }
};

class pmt_complex : public pmt_base
{
  std::complex<double>	d_value;

public:
  pmt_complex(std::complex<double> value);
  //~pmt_complex(){}

  bool is_complex() const { return true; }
  std::complex<double> value() const { return d_value; }
};

class pmt_null  : public pmt_base
{
public:
  pmt_null();
  //~pmt_null(){}

  bool is_null() const { return true; }
};

class pmt_pair : public pmt_base
{
  pmt_t		d_car;
  pmt_t		d_cdr;

public:
  pmt_pair(pmt_t car, pmt_t cdr);
  //~pmt_pair(){};

  bool is_pair() const { return true; }
  pmt_t car() const { return d_car; }
  pmt_t cdr() const { return d_cdr; }

  void set_car(pmt_t car) { d_car = car; }
  void set_cdr(pmt_t cdr) { d_cdr = cdr; }
};

class pmt_vector : public pmt_base
{
  std::vector<pmt_t>	d_v;

public:
  pmt_vector(size_t len, pmt_t fill);
  //~pmt_vector();

  bool is_vector() const { return true; }
  pmt_t ref(size_t k) const;
  void  set(size_t k, pmt_t obj);
  void  fill(pmt_t fill);
  size_t length() const { return d_v.size(); }

  pmt_t _ref(size_t k) const { return d_v[k]; }
};

class pmt_dict : public pmt_base
{
  pmt_t		d_alist;	// list of (key . value) pairs

public:
  pmt_dict();
  //~pmt_dict();

  bool  is_dict() const { return true; }
  void  set(pmt_t key, pmt_t value);
  pmt_t ref(pmt_t key, pmt_t default_value) const;
  bool  has_key(pmt_t key) const;
  pmt_t items() const;
  pmt_t keys() const;
  pmt_t values() const;
};

class pmt_uniform_vector : public pmt_base
{
public:
  bool is_uniform_vector() const { return true; }
  virtual const void *uniform_elements(size_t &len) = 0;
  virtual void *uniform_writeable_elements(size_t &len) = 0;
  virtual size_t length() const = 0;
};

#include "pmt_unv_int.h"

#endif /* INCLUDED_PMT_INT_H */

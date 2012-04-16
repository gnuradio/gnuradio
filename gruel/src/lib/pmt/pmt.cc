/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2010 Free Software Foundation, Inc.
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
#include <config.h>
#endif
#include <vector>
#include <gruel/pmt.h>
#include "pmt_int.h"
#include <gruel/msg_accepter.h>
#include <gruel/pmt_pool.h>
#include <stdio.h>
#include <string.h>

namespace pmt {

static const int CACHE_LINE_SIZE = 64;		// good guess

# if (PMT_LOCAL_ALLOCATOR)

static pmt_pool global_pmt_pool(sizeof(pmt_pair), CACHE_LINE_SIZE);

void *
pmt_base::operator new(size_t size)
{
  void *p = global_pmt_pool.malloc();

  // fprintf(stderr, "pmt_base::new p = %p\n", p);
  assert((reinterpret_cast<intptr_t>(p) & (CACHE_LINE_SIZE - 1)) == 0);
  return p;
}

void
pmt_base::operator delete(void *p, size_t size)
{
  global_pmt_pool.free(p);
}

#endif

void intrusive_ptr_add_ref(pmt_base* p) { ++(p->count_); }
void intrusive_ptr_release(pmt_base* p) { if (--(p->count_) == 0 ) p->deleter_(p); }

pmt_base::~pmt_base()
{
  // nop -- out of line virtual destructor
}

////////////////////////////////////////////////////////////////////////////
//                         Exceptions
////////////////////////////////////////////////////////////////////////////

pmt_exception::pmt_exception(const std::string &msg, pmt_t obj)
  : logic_error(msg + ": " + pmt_write_string(obj))
{
}

pmt_wrong_type::pmt_wrong_type(const std::string &msg, pmt_t obj)
  : pmt_exception(msg + ": wrong_type ", obj)
{
}

pmt_out_of_range::pmt_out_of_range(const std::string &msg, pmt_t obj)
  : pmt_exception(msg + ": out of range ", obj)
{
}

pmt_notimplemented::pmt_notimplemented(const std::string &msg, pmt_t obj)
  : pmt_exception(msg + ": notimplemented ", obj)
{
}

////////////////////////////////////////////////////////////////////////////
//                          Dynamic Casts
////////////////////////////////////////////////////////////////////////////

static pmt_symbol *
_symbol(pmt_t x)
{
  return dynamic_cast<pmt_symbol*>(x.get());
}

static pmt_integer *
_integer(pmt_t x)
{
  return dynamic_cast<pmt_integer*>(x.get());
}

static pmt_uint64 *
_uint64(pmt_t x)
{
  return dynamic_cast<pmt_uint64*>(x.get());
}

static pmt_real *
_real(pmt_t x)
{
  return dynamic_cast<pmt_real*>(x.get());
}

static pmt_complex *
_complex(pmt_t x)
{
  return dynamic_cast<pmt_complex*>(x.get());
}

static pmt_pair *
_pair(pmt_t x)
{
  return dynamic_cast<pmt_pair*>(x.get());
}

static pmt_vector *
_vector(pmt_t x)
{
  return dynamic_cast<pmt_vector*>(x.get());
}

static pmt_tuple *
_tuple(pmt_t x)
{
  return dynamic_cast<pmt_tuple*>(x.get());
}

static pmt_uniform_vector *
_uniform_vector(pmt_t x)
{
  return dynamic_cast<pmt_uniform_vector*>(x.get());
}

static pmt_any *
_any(pmt_t x)
{
  return dynamic_cast<pmt_any*>(x.get());
}

////////////////////////////////////////////////////////////////////////////
//                           Globals
////////////////////////////////////////////////////////////////////////////

const pmt_t PMT_T = pmt_t(new pmt_bool());		// singleton
const pmt_t PMT_F = pmt_t(new pmt_bool());		// singleton
const pmt_t PMT_NIL = pmt_t(new pmt_null());		// singleton
const pmt_t PMT_EOF = pmt_cons(PMT_NIL, PMT_NIL);	// singleton

////////////////////////////////////////////////////////////////////////////
//                           Booleans
////////////////////////////////////////////////////////////////////////////

pmt_bool::pmt_bool(){}

bool
pmt_is_true(pmt_t obj)
{
  return obj != PMT_F;
}

bool
pmt_is_false(pmt_t obj)
{
  return obj == PMT_F;
}

bool
pmt_is_bool(pmt_t obj)
{
  return obj->is_bool();
}

pmt_t
pmt_from_bool(bool val)
{
  return val ? PMT_T : PMT_F;
}

bool
pmt_to_bool(pmt_t val)
{
  if (val == PMT_T)
    return true;
  if (val == PMT_F)
    return false;
  throw pmt_wrong_type("pmt_to_bool", val);
}

////////////////////////////////////////////////////////////////////////////
//                             Symbols
////////////////////////////////////////////////////////////////////////////

static const unsigned int SYMBOL_HASH_TABLE_SIZE = 701;
static std::vector<pmt_t> s_symbol_hash_table(SYMBOL_HASH_TABLE_SIZE);

pmt_symbol::pmt_symbol(const std::string &name) : d_name(name){}


static unsigned int
hash_string(const std::string &s)
{
  unsigned int h = 0;
  unsigned int g = 0;

  for (std::string::const_iterator p = s.begin(); p != s.end(); ++p){
    h = (h << 4) + (*p & 0xff);
    g = h & 0xf0000000;
    if (g){
      h = h ^ (g >> 24);
      h = h ^ g;
    }
  }
  return h;
}

bool 
pmt_is_symbol(const pmt_t& obj)
{
  return obj->is_symbol();
}

pmt_t 
pmt_string_to_symbol(const std::string &name)
{
  unsigned hash = hash_string(name) % SYMBOL_HASH_TABLE_SIZE;

  // Does a symbol with this name already exist?
  for (pmt_t sym = s_symbol_hash_table[hash]; sym; sym = _symbol(sym)->next()){
    if (name == _symbol(sym)->name())
      return sym;		// Yes.  Return it
  }

  // Nope.  Make a new one.
  pmt_t sym = pmt_t(new pmt_symbol(name));
  _symbol(sym)->set_next(s_symbol_hash_table[hash]);
  s_symbol_hash_table[hash] = sym;
  return sym;
}

// alias...
pmt_t
pmt_intern(const std::string &name)
{
  return pmt_string_to_symbol(name);
}

const std::string
pmt_symbol_to_string(const pmt_t& sym)
{
  if (!sym->is_symbol())
    throw pmt_wrong_type("pmt_symbol_to_string", sym);

  return _symbol(sym)->name();
}



////////////////////////////////////////////////////////////////////////////
//                             Number
////////////////////////////////////////////////////////////////////////////

bool
pmt_is_number(pmt_t x)
{
  return x->is_number();
}

////////////////////////////////////////////////////////////////////////////
//                             Integer
////////////////////////////////////////////////////////////////////////////

pmt_integer::pmt_integer(long value) : d_value(value) {}

bool
pmt_is_integer(pmt_t x)
{
  return x->is_integer();
}


pmt_t
pmt_from_long(long x)
{
  return pmt_t(new pmt_integer(x));
}

long
pmt_to_long(pmt_t x)
{
  pmt_integer* i = dynamic_cast<pmt_integer*>(x.get());
  if ( i )
    return i->value();

  throw pmt_wrong_type("pmt_to_long", x);
}

////////////////////////////////////////////////////////////////////////////
//                             Uint64
////////////////////////////////////////////////////////////////////////////

pmt_uint64::pmt_uint64(uint64_t value) : d_value(value) {}

bool
pmt_is_uint64(pmt_t x)
{
  return x->is_uint64();
}


pmt_t
pmt_from_uint64(uint64_t x)
{
  return pmt_t(new pmt_uint64(x));
}

uint64_t
pmt_to_uint64(pmt_t x)
{
  if(x->is_uint64())
    return _uint64(x)->value();
  if(x->is_integer())
    {
    long tmp = _integer(x)->value();
    if(tmp >= 0)
        return (uint64_t) tmp;
    }

  throw pmt_wrong_type("pmt_to_uint64", x);
}

////////////////////////////////////////////////////////////////////////////
//                              Real
////////////////////////////////////////////////////////////////////////////

pmt_real::pmt_real(double value) : d_value(value) {}

bool 
pmt_is_real(pmt_t x)
{
  return x->is_real();
}

pmt_t
pmt_from_double(double x)
{
  return pmt_t(new pmt_real(x));
}

double
pmt_to_double(pmt_t x)
{
  if (x->is_real())
    return _real(x)->value();
  if (x->is_integer())
    return _integer(x)->value();

  throw pmt_wrong_type("pmt_to_double", x);
}

////////////////////////////////////////////////////////////////////////////
//                              Complex
////////////////////////////////////////////////////////////////////////////

pmt_complex::pmt_complex(std::complex<double> value) : d_value(value) {}

bool 
pmt_is_complex(pmt_t x)
{
  return x->is_complex();
}

pmt_t
pmt_make_rectangular(double re, double im)
{
  return pmt_t(new pmt_complex(std::complex<double>(re, im)));
}

std::complex<double>
pmt_to_complex(pmt_t x)
{
  if (x->is_complex())
    return _complex(x)->value();
  if (x->is_real())
    return _real(x)->value();
  if (x->is_integer())
    return _integer(x)->value();

  throw pmt_wrong_type("pmt_to_complex", x);
}

////////////////////////////////////////////////////////////////////////////
//                              Pairs
////////////////////////////////////////////////////////////////////////////

pmt_null::pmt_null() {}
pmt_pair::pmt_pair(const pmt_t& car, const pmt_t& cdr) : d_car(car), d_cdr(cdr) {}

bool
pmt_is_null(const pmt_t& x)
{
  return x == PMT_NIL;
}

bool
pmt_is_pair(const pmt_t& obj)
{
  return obj->is_pair();
}

pmt_t
pmt_cons(const pmt_t& x, const pmt_t& y)
{
  return pmt_t(new pmt_pair(x, y));
}

pmt_t
pmt_car(const pmt_t& pair)
{
  pmt_pair* p = dynamic_cast<pmt_pair*>(pair.get());
  if ( p )
    return p->car();
  
  throw pmt_wrong_type("pmt_car", pair);
}

pmt_t
pmt_cdr(const pmt_t& pair)
{
  pmt_pair* p = dynamic_cast<pmt_pair*>(pair.get());
  if ( p )
    return p->cdr();
  
  throw pmt_wrong_type("pmt_cdr", pair);
}

void
pmt_set_car(pmt_t pair, pmt_t obj)
{
  if (pair->is_pair())
    _pair(pair)->set_car(obj);
  else
    throw pmt_wrong_type("pmt_set_car", pair);
}

void
pmt_set_cdr(pmt_t pair, pmt_t obj)
{
  if (pair->is_pair())
    _pair(pair)->set_cdr(obj);
  else
    throw pmt_wrong_type("pmt_set_cdr", pair);
}

////////////////////////////////////////////////////////////////////////////
//                             Vectors
////////////////////////////////////////////////////////////////////////////

pmt_vector::pmt_vector(size_t len, pmt_t fill)
  : d_v(len)
{
  for (size_t i = 0; i < len; i++)
    d_v[i] = fill;
}

pmt_t
pmt_vector::ref(size_t k) const
{
  if (k >= length())
    throw pmt_out_of_range("pmt_vector_ref", pmt_from_long(k));
  return d_v[k];
}

void
pmt_vector::set(size_t k, pmt_t obj)
{
  if (k >= length())
    throw pmt_out_of_range("pmt_vector_set", pmt_from_long(k));
  d_v[k] = obj;
}

void
pmt_vector::fill(pmt_t obj)
{
  for (size_t i = 0; i < length(); i++)
    d_v[i] = obj;
}

bool
pmt_is_vector(pmt_t obj)
{
  return obj->is_vector();
}

pmt_t
pmt_make_vector(size_t k, pmt_t fill)
{
  return pmt_t(new pmt_vector(k, fill));
}

pmt_t
pmt_vector_ref(pmt_t vector, size_t k)
{
  if (!vector->is_vector())
    throw pmt_wrong_type("pmt_vector_ref", vector);
  return _vector(vector)->ref(k);
}

void
pmt_vector_set(pmt_t vector, size_t k, pmt_t obj)
{
  if (!vector->is_vector())
    throw pmt_wrong_type("pmt_vector_set", vector);
  _vector(vector)->set(k, obj);
}

void
pmt_vector_fill(pmt_t vector, pmt_t obj)
{
  if (!vector->is_vector())
    throw pmt_wrong_type("pmt_vector_set", vector);
  _vector(vector)->fill(obj);
}

////////////////////////////////////////////////////////////////////////////
//                             Tuples
////////////////////////////////////////////////////////////////////////////

pmt_tuple::pmt_tuple(size_t len)
  : d_v(len)
{
}

pmt_t
pmt_tuple::ref(size_t k) const
{
  if (k >= length())
    throw pmt_out_of_range("pmt_tuple_ref", pmt_from_long(k));
  return d_v[k];
}

bool
pmt_is_tuple(pmt_t obj)
{
  return obj->is_tuple();
}

pmt_t
pmt_tuple_ref(const pmt_t &tuple, size_t k)
{
  if (!tuple->is_tuple())
    throw pmt_wrong_type("pmt_tuple_ref", tuple);
  return _tuple(tuple)->ref(k);
}

// for (i=0; i < 10; i++)
//   make_constructor()

pmt_t
pmt_make_tuple()
{
  return pmt_t(new pmt_tuple(0));
}

pmt_t
pmt_make_tuple(const pmt_t &e0)
{
  pmt_tuple *t = new pmt_tuple(1);
  t->_set(0, e0);
  return pmt_t(t);
}

pmt_t
pmt_make_tuple(const pmt_t &e0, const pmt_t &e1)
{
  pmt_tuple *t = new pmt_tuple(2);
  t->_set(0, e0);
  t->_set(1, e1);
  return pmt_t(t);
}

pmt_t
pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2)
{
  pmt_tuple *t = new pmt_tuple(3);
  t->_set(0, e0);
  t->_set(1, e1);
  t->_set(2, e2);
  return pmt_t(t);
}

pmt_t
pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3)
{
  pmt_tuple *t = new pmt_tuple(4);
  t->_set(0, e0);
  t->_set(1, e1);
  t->_set(2, e2);
  t->_set(3, e3);
  return pmt_t(t);
}

pmt_t
pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4)
{
  pmt_tuple *t = new pmt_tuple(5);
  t->_set(0, e0);
  t->_set(1, e1);
  t->_set(2, e2);
  t->_set(3, e3);
  t->_set(4, e4);
  return pmt_t(t);
}

pmt_t
pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5)
{
  pmt_tuple *t = new pmt_tuple(6);
  t->_set(0, e0);
  t->_set(1, e1);
  t->_set(2, e2);
  t->_set(3, e3);
  t->_set(4, e4);
  t->_set(5, e5);
  return pmt_t(t);
}

pmt_t
pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6)
{
  pmt_tuple *t = new pmt_tuple(7);
  t->_set(0, e0);
  t->_set(1, e1);
  t->_set(2, e2);
  t->_set(3, e3);
  t->_set(4, e4);
  t->_set(5, e5);
  t->_set(6, e6);
  return pmt_t(t);
}

pmt_t
pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7)
{
  pmt_tuple *t = new pmt_tuple(8);
  t->_set(0, e0);
  t->_set(1, e1);
  t->_set(2, e2);
  t->_set(3, e3);
  t->_set(4, e4);
  t->_set(5, e5);
  t->_set(6, e6);
  t->_set(7, e7);
  return pmt_t(t);
}

pmt_t
pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7, const pmt_t &e8)
{
  pmt_tuple *t = new pmt_tuple(9);
  t->_set(0, e0);
  t->_set(1, e1);
  t->_set(2, e2);
  t->_set(3, e3);
  t->_set(4, e4);
  t->_set(5, e5);
  t->_set(6, e6);
  t->_set(7, e7);
  t->_set(8, e8);
  return pmt_t(t);
}

pmt_t
pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7, const pmt_t &e8, const pmt_t &e9)
{
  pmt_tuple *t = new pmt_tuple(10);
  t->_set(0, e0);
  t->_set(1, e1);
  t->_set(2, e2);
  t->_set(3, e3);
  t->_set(4, e4);
  t->_set(5, e5);
  t->_set(6, e6);
  t->_set(7, e7);
  t->_set(8, e8);
  t->_set(9, e9);
  return pmt_t(t);
}

pmt_t
pmt_to_tuple(const pmt_t &x)
{
  if (x->is_tuple())		// already one
    return x;

  size_t len = pmt_length(x);
  pmt_tuple *t = new pmt_tuple(len);
  pmt_t r = pmt_t(t);

  if (x->is_vector()){
    for (size_t i = 0; i < len; i++)
      t->_set(i, _vector(x)->ref(i));
    return r;
  }

  if (x->is_pair()){
    pmt_t y = x;
    for (size_t i = 0; i < len; i++){
      t->_set(i, pmt_car(y));
      y = pmt_cdr(y);
    }
    return r;
  }

  throw pmt_wrong_type("pmt_to_tuple", x);
}



////////////////////////////////////////////////////////////////////////////
//                       Uniform Numeric Vectors
////////////////////////////////////////////////////////////////////////////

bool
pmt_is_uniform_vector(pmt_t x)
{
  return x->is_uniform_vector();
}

const void *
pmt_uniform_vector_elements(pmt_t vector, size_t &len)
{
  if (!vector->is_uniform_vector())
    throw pmt_wrong_type("pmt_uniform_vector_elements", vector);
  return _uniform_vector(vector)->uniform_elements(len);
}

void *
pmt_uniform_vector_writable_elements(pmt_t vector, size_t &len)
{
  if (!vector->is_uniform_vector())
    throw pmt_wrong_type("pmt_uniform_vector_writable_elements", vector);
  return _uniform_vector(vector)->uniform_writable_elements(len);
}

////////////////////////////////////////////////////////////////////////////
//                            Dictionaries
////////////////////////////////////////////////////////////////////////////

/*
 * This is an a-list implementation.
 *
 * When we need better performance for large dictionaries, consider implementing
 * persistent Red-Black trees as described in "Purely Functional Data Structures", 
 * Chris Okasaki, 1998, section 3.3.
 */

bool
pmt_is_dict(const pmt_t &obj)
{
  return pmt_is_null(obj) || pmt_is_pair(obj);
}

pmt_t
pmt_make_dict()
{
  return PMT_NIL;
}

pmt_t
pmt_dict_add(const pmt_t &dict, const pmt_t &key, const pmt_t &value)
{
  if (pmt_is_null(dict))
    return pmt_acons(key, value, PMT_NIL);

  if (pmt_dict_has_key(dict, key))
    return pmt_acons(key, value, pmt_dict_delete(dict, key));

  return pmt_acons(key, value, dict);
}

pmt_t
pmt_dict_delete(const pmt_t &dict, const pmt_t &key)
{
  if (pmt_is_null(dict))
    return dict;

  if (pmt_eqv(pmt_caar(dict), key))
    return pmt_cdr(dict);
  
  return pmt_cons(pmt_car(dict), pmt_dict_delete(pmt_cdr(dict), key));
}

pmt_t
pmt_dict_ref(const pmt_t &dict, const pmt_t &key, const pmt_t &not_found)
{
  pmt_t	p = pmt_assv(key, dict);	// look for (key . value) pair
  if (pmt_is_pair(p))
    return pmt_cdr(p);
  else
    return not_found;
}

bool
pmt_dict_has_key(const pmt_t &dict, const pmt_t &key)
{
  return pmt_is_pair(pmt_assv(key, dict));
}

pmt_t
pmt_dict_items(pmt_t dict)
{
  if (!pmt_is_dict(dict))
    throw pmt_wrong_type("pmt_dict_values", dict);

  return dict;		// equivalent to dict in the a-list case
}

pmt_t
pmt_dict_keys(pmt_t dict)
{
  if (!pmt_is_dict(dict))
    throw pmt_wrong_type("pmt_dict_keys", dict);

  return pmt_map(pmt_car, dict);
}

pmt_t
pmt_dict_values(pmt_t dict)
{
  if (!pmt_is_dict(dict))
    throw pmt_wrong_type("pmt_dict_keys", dict);

  return pmt_map(pmt_cdr, dict);
}

////////////////////////////////////////////////////////////////////////////
//                                 Any
////////////////////////////////////////////////////////////////////////////

pmt_any::pmt_any(const boost::any &any) : d_any(any) {}

bool
pmt_is_any(pmt_t obj)
{
  return obj->is_any();
}

pmt_t
pmt_make_any(const boost::any &any)
{
  return pmt_t(new pmt_any(any));
}

boost::any
pmt_any_ref(pmt_t obj)
{
  if (!obj->is_any())
    throw pmt_wrong_type("pmt_any_ref", obj);
  return _any(obj)->ref();
}

void
pmt_any_set(pmt_t obj, const boost::any &any)
{
  if (!obj->is_any())
    throw pmt_wrong_type("pmt_any_set", obj);
  _any(obj)->set(any);
}

////////////////////////////////////////////////////////////////////////////
//               msg_accepter -- built from "any"
////////////////////////////////////////////////////////////////////////////

bool 
pmt_is_msg_accepter(const pmt_t &obj)
{
  if (!pmt_is_any(obj))
    return false;

  boost::any r = pmt_any_ref(obj);
  return boost::any_cast<gruel::msg_accepter_sptr>(&r) != 0;
}

//! make a msg_accepter
pmt_t
pmt_make_msg_accepter(gruel::msg_accepter_sptr ma)
{
  return pmt_make_any(ma);
}

//! Return underlying msg_accepter
gruel::msg_accepter_sptr
pmt_msg_accepter_ref(const pmt_t &obj)
{
  try {
    return boost::any_cast<gruel::msg_accepter_sptr>(pmt_any_ref(obj));
  }
  catch (boost::bad_any_cast &e){
    throw pmt_wrong_type("pmt_msg_accepter_ref", obj);
  }
}


////////////////////////////////////////////////////////////////////////////
//             Binary Large Object -- currently a u8vector
////////////////////////////////////////////////////////////////////////////

bool
pmt_is_blob(pmt_t x)
{
  // return pmt_is_u8vector(x);
  return pmt_is_uniform_vector(x);
}

pmt_t
pmt_make_blob(const void *buf, size_t len_in_bytes)
{
  return pmt_init_u8vector(len_in_bytes, (const uint8_t *) buf);
}

const void *
pmt_blob_data(pmt_t blob)
{
  size_t len;
  return pmt_uniform_vector_elements(blob, len);
}

size_t
pmt_blob_length(pmt_t blob)
{
  size_t len;
  pmt_uniform_vector_elements(blob, len);
  return len;
}


////////////////////////////////////////////////////////////////////////////
//                          General Functions
////////////////////////////////////////////////////////////////////////////

bool
pmt_eq(const pmt_t& x, const pmt_t& y)
{
  return x == y;
}

bool
pmt_eqv(const pmt_t& x, const pmt_t& y)
{
  if (x == y)
    return true;

  if (x->is_integer() && y->is_integer())
    return _integer(x)->value() == _integer(y)->value();

  if (x->is_uint64() && y->is_uint64())
    return _uint64(x)->value() == _uint64(y)->value();

  if (x->is_real() && y->is_real())
    return _real(x)->value() == _real(y)->value();

  if (x->is_complex() && y->is_complex())
    return _complex(x)->value() == _complex(y)->value();

  return false;
}

bool
pmt_eqv_raw(pmt_base *x, pmt_base *y)
{
  if (x == y)
    return true;

  if (x->is_integer() && y->is_integer())
    return _integer(x)->value() == _integer(y)->value();

  if (x->is_uint64() && y->is_uint64())
    return _uint64(x)->value() == _uint64(y)->value();

  if (x->is_real() && y->is_real())
    return _real(x)->value() == _real(y)->value();

  if (x->is_complex() && y->is_complex())
    return _complex(x)->value() == _complex(y)->value();

  return false;
}

bool
pmt_equal(const pmt_t& x, const pmt_t& y)
{
  if (pmt_eqv(x, y))
    return true;

  if (x->is_pair() && y->is_pair())
    return pmt_equal(pmt_car(x), pmt_car(y)) && pmt_equal(pmt_cdr(x), pmt_cdr(y));

  if (x->is_vector() && y->is_vector()){
    pmt_vector *xv = _vector(x);
    pmt_vector *yv = _vector(y);
    if (xv->length() != yv->length())
      return false;

    for (unsigned i = 0; i < xv->length(); i++)
      if (!pmt_equal(xv->_ref(i), yv->_ref(i)))
	return false;

    return true;
  }

  if (x->is_tuple() && y->is_tuple()){
    pmt_tuple *xv = _tuple(x);
    pmt_tuple *yv = _tuple(y);
    if (xv->length() != yv->length())
      return false;

    for (unsigned i = 0; i < xv->length(); i++)
      if (!pmt_equal(xv->_ref(i), yv->_ref(i)))
	return false;

    return true;
  }

  if (x->is_uniform_vector() && y->is_uniform_vector()){
    pmt_uniform_vector *xv = _uniform_vector(x);
    pmt_uniform_vector *yv = _uniform_vector(y);
    if (xv->length() != yv->length())
      return false;

    size_t len_x, len_y;
    if (memcmp(xv->uniform_elements(len_x),
	       yv->uniform_elements(len_y),
	       len_x) == 0)
      return true;

    return true;
  }

  // FIXME add other cases here...

  return false;
}

size_t
pmt_length(const pmt_t& x)
{
  if (x->is_vector())
    return _vector(x)->length();

  if (x->is_uniform_vector())
    return _uniform_vector(x)->length();

  if (x->is_tuple())
    return _tuple(x)->length();

  if (x->is_null())
    return 0;

  if (x->is_pair()) {
    size_t length=1;
    pmt_t it = pmt_cdr(x);
    while (pmt_is_pair(it)){
      length++;
      it = pmt_cdr(it);
    }
    if (pmt_is_null(it))
      return length;

    // not a proper list
    throw pmt_wrong_type("pmt_length", x);
  }

  // FIXME dictionary length (number of entries)

  throw pmt_wrong_type("pmt_length", x);
}

pmt_t
pmt_assq(pmt_t obj, pmt_t alist)
{
  while (pmt_is_pair(alist)){
    pmt_t p = pmt_car(alist);
    if (!pmt_is_pair(p))	// malformed alist
      return PMT_F;

    if (pmt_eq(obj, pmt_car(p)))
      return p;

    alist = pmt_cdr(alist);
  }
  return PMT_F;
}

/*
 * This avoids a bunch of shared_pointer reference count manipulation.
 */
pmt_t
pmt_assv_raw(pmt_base *obj, pmt_base *alist)
{
  while (alist->is_pair()){
    pmt_base *p = ((pmt_pair *)alist)->d_car.get();
    if (!p->is_pair())		// malformed alist
      return PMT_F;

    if (pmt_eqv_raw(obj, ((pmt_pair *)p)->d_car.get()))
      return ((pmt_pair *)alist)->d_car;

    alist = (((pmt_pair *)alist)->d_cdr).get();
  }
  return PMT_F;
}

#if 1

pmt_t
pmt_assv(pmt_t obj, pmt_t alist)
{
  return pmt_assv_raw(obj.get(), alist.get());
}

#else

pmt_t
pmt_assv(pmt_t obj, pmt_t alist)
{
  while (pmt_is_pair(alist)){
    pmt_t p = pmt_car(alist);
    if (!pmt_is_pair(p))	// malformed alist
      return PMT_F;

    if (pmt_eqv(obj, pmt_car(p)))
      return p;

    alist = pmt_cdr(alist);
  }
  return PMT_F;
}

#endif


pmt_t
pmt_assoc(pmt_t obj, pmt_t alist)
{
  while (pmt_is_pair(alist)){
    pmt_t p = pmt_car(alist);
    if (!pmt_is_pair(p))	// malformed alist
      return PMT_F;

    if (pmt_equal(obj, pmt_car(p)))
      return p;

    alist = pmt_cdr(alist);
  }
  return PMT_F;
}

pmt_t
pmt_map(pmt_t proc(const pmt_t&), pmt_t list)
{
  pmt_t r = PMT_NIL;

  while(pmt_is_pair(list)){
    r = pmt_cons(proc(pmt_car(list)), r);
    list = pmt_cdr(list);
  }

  return pmt_reverse_x(r);
}

pmt_t
pmt_reverse(pmt_t listx)
{
  pmt_t list = listx;
  pmt_t r = PMT_NIL;

  while(pmt_is_pair(list)){
    r = pmt_cons(pmt_car(list), r);
    list = pmt_cdr(list);
  }
  if (pmt_is_null(list))
    return r;
  else
    throw pmt_wrong_type("pmt_reverse", listx);
}

pmt_t
pmt_reverse_x(pmt_t list)
{
  // FIXME do it destructively
  return pmt_reverse(list);
}

pmt_t
pmt_nth(size_t n, pmt_t list)
{
  pmt_t t = pmt_nthcdr(n, list);
  if (pmt_is_pair(t))
    return pmt_car(t);
  else
    return PMT_NIL;
}

pmt_t
pmt_nthcdr(size_t n, pmt_t list)
{
  if (!(pmt_is_pair(list) || pmt_is_null(list)))
    throw pmt_wrong_type("pmt_nthcdr", list);
    
  while (n > 0){
    if (pmt_is_pair(list)){
      list = pmt_cdr(list);
      n--;
      continue;
    }
    if (pmt_is_null(list))
      return PMT_NIL;
    else
      throw pmt_wrong_type("pmt_nthcdr: not a LIST", list);
  }
  return list;
}

pmt_t
pmt_memq(pmt_t obj, pmt_t list)
{
  while (pmt_is_pair(list)){
    if (pmt_eq(obj, pmt_car(list)))
      return list;
    list = pmt_cdr(list);
  }
  return PMT_F;
}

pmt_t
pmt_memv(pmt_t obj, pmt_t list)
{
  while (pmt_is_pair(list)){
    if (pmt_eqv(obj, pmt_car(list)))
      return list;
    list = pmt_cdr(list);
  }
  return PMT_F;
}

pmt_t
pmt_member(pmt_t obj, pmt_t list)
{
  while (pmt_is_pair(list)){
    if (pmt_equal(obj, pmt_car(list)))
      return list;
    list = pmt_cdr(list);
  }
  return PMT_F;
}

bool
pmt_subsetp(pmt_t list1, pmt_t list2)
{
  while (pmt_is_pair(list1)){
    pmt_t p = pmt_car(list1);
    if (pmt_is_false(pmt_memv(p, list2)))
      return false;
    list1 = pmt_cdr(list1);
  }
  return true;
}

pmt_t
pmt_list1(const pmt_t& x1)
{
  return pmt_cons(x1, PMT_NIL);
}

pmt_t
pmt_list2(const pmt_t& x1, const pmt_t& x2)
{
  return pmt_cons(x1, pmt_cons(x2, PMT_NIL));
}

pmt_t
pmt_list3(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3)
{
  return pmt_cons(x1, pmt_cons(x2, pmt_cons(x3, PMT_NIL)));
}

pmt_t
pmt_list4(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4)
{
  return pmt_cons(x1, pmt_cons(x2, pmt_cons(x3, pmt_cons(x4, PMT_NIL))));
}

pmt_t
pmt_list5(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4, const pmt_t& x5)
{
  return pmt_cons(x1, pmt_cons(x2, pmt_cons(x3, pmt_cons(x4, pmt_cons(x5, PMT_NIL)))));
}

pmt_t
pmt_list6(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4, const pmt_t& x5, const pmt_t& x6)
{
  return pmt_cons(x1, pmt_cons(x2, pmt_cons(x3, pmt_cons(x4, pmt_cons(x5, pmt_cons(x6, PMT_NIL))))));
}

pmt_t
pmt_list_add(pmt_t list, const pmt_t& item)
{
  return pmt_reverse(pmt_cons(item, pmt_reverse(list)));
}

pmt_t
pmt_caar(pmt_t pair)
{
  return (pmt_car(pmt_car(pair)));
}

pmt_t
pmt_cadr(pmt_t pair)
{
  return pmt_car(pmt_cdr(pair));
}

pmt_t
pmt_cdar(pmt_t pair)
{
  return pmt_cdr(pmt_car(pair));
}

pmt_t
pmt_cddr(pmt_t pair)
{
  return pmt_cdr(pmt_cdr(pair));
}

pmt_t
pmt_caddr(pmt_t pair)
{
  return pmt_car(pmt_cdr(pmt_cdr(pair)));
}

pmt_t
pmt_cadddr(pmt_t pair)
{
  return pmt_car(pmt_cdr(pmt_cdr(pmt_cdr(pair))));
}
  
bool
pmt_is_eof_object(pmt_t obj)
{
  return pmt_eq(obj, PMT_EOF);
}

void
pmt_dump_sizeof()
{
  printf("sizeof(pmt_t)              = %3zd\n", sizeof(pmt_t));
  printf("sizeof(pmt_base)           = %3zd\n", sizeof(pmt_base));
  printf("sizeof(pmt_bool)           = %3zd\n", sizeof(pmt_bool));
  printf("sizeof(pmt_symbol)         = %3zd\n", sizeof(pmt_symbol));
  printf("sizeof(pmt_integer)        = %3zd\n", sizeof(pmt_integer));
  printf("sizeof(pmt_uint64)         = %3zd\n", sizeof(pmt_uint64));
  printf("sizeof(pmt_real)           = %3zd\n", sizeof(pmt_real));
  printf("sizeof(pmt_complex)        = %3zd\n", sizeof(pmt_complex));
  printf("sizeof(pmt_null)           = %3zd\n", sizeof(pmt_null));
  printf("sizeof(pmt_pair)           = %3zd\n", sizeof(pmt_pair));
  printf("sizeof(pmt_vector)         = %3zd\n", sizeof(pmt_vector));
  printf("sizeof(pmt_uniform_vector) = %3zd\n", sizeof(pmt_uniform_vector));
}

/*
 * ------------------------------------------------------------------------
 *		      advanced
 * ------------------------------------------------------------------------
 */

void
pmt_set_deleter(pmt_t obj, boost::function<void(pmt_base *)> &deleter)
{
  obj->deleter_ = (deleter)? deleter : &pmt_base::default_deleter;
}

} /* namespace pmt */

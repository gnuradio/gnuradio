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
#include <pmt/pmt.h>
#include "pmt_int.h"
#include <gnuradio/messages/msg_accepter.h>
#include <pmt/pmt_pool.h>
#include <stdio.h>
#include <string.h>

namespace pmt {

# if (PMT_LOCAL_ALLOCATOR)

static const int
get_cache_line_size()
{
  static const int CACHE_LINE_SIZE = 64;		// good guess
  return CACHE_LINE_SIZE;
}

static pmt_pool global_pmt_pool(sizeof(pmt_pair), get_cache_line_size());

void *
pmt_base::operator new(size_t size)
{
  void *p = global_pmt_pool.malloc();

  // fprintf(stderr, "pmt_base::new p = %p\n", p);
  assert((reinterpret_cast<intptr_t>(p) & (get_cache_line_size() - 1)) == 0);
  return p;
}

void
pmt_base::operator delete(void *p, size_t size)
{
  global_pmt_pool.free(p);
}

#endif

#if ((BOOST_VERSION / 100000 >= 1) && (BOOST_VERSION / 100 % 1000 >= 53)) 
void intrusive_ptr_add_ref(pmt_base* p)
{
  p->refcount_.fetch_add(1, boost::memory_order_relaxed);
}

void intrusive_ptr_release(pmt_base* p) {
  if (p->refcount_.fetch_sub(1, boost::memory_order_release) == 1) {
    boost::atomic_thread_fence(boost::memory_order_acquire);
    delete p;
  }
}
#else
// boost::atomic not available before 1.53
// This section will be removed when support for boost 1.48 ceases
// NB: This code is prone to segfault on non-Intel architectures.
void intrusive_ptr_add_ref(pmt_base* p) { ++(p->count_); }
void intrusive_ptr_release(pmt_base* p) { if (--(p->count_) == 0 ) delete p; }
#endif
 
pmt_base::~pmt_base()
{
  // nop -- out of line virtual destructor
}

////////////////////////////////////////////////////////////////////////////
//                         Exceptions
////////////////////////////////////////////////////////////////////////////

exception::exception(const std::string &msg, pmt_t obj)
  : logic_error(msg + ": " + write_string(obj))
{
}

wrong_type::wrong_type(const std::string &msg, pmt_t obj)
  : exception(msg + ": wrong_type ", obj)
{
}

out_of_range::out_of_range(const std::string &msg, pmt_t obj)
  : exception(msg + ": out of range ", obj)
{
}

notimplemented::notimplemented(const std::string &msg, pmt_t obj)
  : exception(msg + ": notimplemented ", obj)
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

pmt_t get_PMT_NIL()
{
  static pmt_t _NIL = pmt_t(new pmt_null());
  return _NIL;
}

pmt_t get_PMT_T()
{
  static const pmt_t _T = pmt_t(new pmt_bool());
  return _T;
}

pmt_t get_PMT_F()
{
  static const pmt_t _F = pmt_t(new pmt_bool());
  return _F;
}

pmt_t get_PMT_EOF()
{
  static const pmt_t _EOF = cons(get_PMT_NIL(), get_PMT_NIL());
  return _EOF;
}

////////////////////////////////////////////////////////////////////////////
//                           Booleans
////////////////////////////////////////////////////////////////////////////

pmt_bool::pmt_bool(){}

bool
is_true(pmt_t obj)
{
  return obj != PMT_F;
}

bool
is_false(pmt_t obj)
{
  return obj == PMT_F;
}

bool
is_bool(pmt_t obj)
{
  return obj->is_bool();
}

pmt_t
from_bool(bool val)
{
  return val ? PMT_T : PMT_F;
}

bool
to_bool(pmt_t val)
{
  if (val == PMT_T)
    return true;
  if (val == PMT_F)
    return false;
  throw wrong_type("pmt_to_bool", val);
}

////////////////////////////////////////////////////////////////////////////
//                             Symbols
////////////////////////////////////////////////////////////////////////////

static const unsigned int
get_symbol_hash_table_size()
{
  static const unsigned int SYMBOL_HASH_TABLE_SIZE = 701;
  return SYMBOL_HASH_TABLE_SIZE;
}

static std::vector<pmt_t>*
get_symbol_hash_table()
{
  static std::vector<pmt_t> s_symbol_hash_table(get_symbol_hash_table_size());
  return &s_symbol_hash_table;
}

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
is_symbol(const pmt_t& obj)
{
  return obj->is_symbol();
}

pmt_t
string_to_symbol(const std::string &name)
{
  unsigned hash = hash_string(name) % get_symbol_hash_table_size();

  // Does a symbol with this name already exist?
  for (pmt_t sym = (*get_symbol_hash_table())[hash]; sym; sym = _symbol(sym)->next()){
    if (name == _symbol(sym)->name())
      return sym;		// Yes.  Return it
  }
  
  // Lock the table on insert for thread safety:
  static boost::mutex thread_safety;
  boost::mutex::scoped_lock lock(thread_safety);
  // Re-do the search in case another thread inserted this symbol into the table
  // before we got the lock
  for (pmt_t sym = (*get_symbol_hash_table())[hash]; sym; sym = _symbol(sym)->next()){
    if (name == _symbol(sym)->name())
      return sym;		// Yes.  Return it
  }
  
  // Nope.  Make a new one.
  pmt_t sym = pmt_t(new pmt_symbol(name));
  _symbol(sym)->set_next((*get_symbol_hash_table())[hash]);
  (*get_symbol_hash_table())[hash] = sym;
  return sym;
}

// alias...
pmt_t
intern(const std::string &name)
{
  return string_to_symbol(name);
}

const std::string
symbol_to_string(const pmt_t& sym)
{
  if (!sym->is_symbol())
    throw wrong_type("pmt_symbol_to_string", sym);

  return _symbol(sym)->name();
}



////////////////////////////////////////////////////////////////////////////
//                             Number
////////////////////////////////////////////////////////////////////////////

bool
is_number(pmt_t x)
{
  return x->is_number();
}

////////////////////////////////////////////////////////////////////////////
//                             Integer
////////////////////////////////////////////////////////////////////////////

pmt_integer::pmt_integer(long value) : d_value(value) {}

bool
is_integer(pmt_t x)
{
  return x->is_integer();
}


pmt_t
from_long(long x)
{
  return pmt_t(new pmt_integer(x));
}

long
to_long(pmt_t x)
{
  pmt_integer* i = dynamic_cast<pmt_integer*>(x.get());
  if ( i )
    return i->value();

  throw wrong_type("pmt_to_long", x);
}

////////////////////////////////////////////////////////////////////////////
//                             Uint64
////////////////////////////////////////////////////////////////////////////

pmt_uint64::pmt_uint64(uint64_t value) : d_value(value) {}

bool
is_uint64(pmt_t x)
{
  return x->is_uint64();
}


pmt_t
from_uint64(uint64_t x)
{
  return pmt_t(new pmt_uint64(x));
}

uint64_t
to_uint64(pmt_t x)
{
  if(x->is_uint64())
    return _uint64(x)->value();
  if(x->is_integer())
    {
    long tmp = _integer(x)->value();
    if(tmp >= 0)
        return (uint64_t) tmp;
    }

  throw wrong_type("pmt_to_uint64", x);
}

////////////////////////////////////////////////////////////////////////////
//                              Real
////////////////////////////////////////////////////////////////////////////

pmt_real::pmt_real(double value) : d_value(value) {}

bool
is_real(pmt_t x)
{
  return x->is_real();
}

pmt_t
from_double(double x)
{
  return pmt_t(new pmt_real(x));
}

pmt_t
from_float(float x)
{
  return pmt_t(new pmt_real(x));
}

double
to_double(pmt_t x)
{
  if (x->is_real())
    return _real(x)->value();
  if (x->is_integer())
    return _integer(x)->value();

  throw wrong_type("pmt_to_double", x);
}

float
to_float(pmt_t x)
{
  return float(to_double(x));
}

////////////////////////////////////////////////////////////////////////////
//                              Complex
////////////////////////////////////////////////////////////////////////////

pmt_complex::pmt_complex(std::complex<double> value) : d_value(value) {}

bool
is_complex(pmt_t x)
{
  return x->is_complex();
}

pmt_t
make_rectangular(double re, double im)
{
  return from_complex(re, im);
}

pmt_t
from_complex(double re, double im)
{
  return pmt_from_complex(re, im);
}

pmt_t pmt_from_complex(double re, double im)
{
  return pmt_t(new pmt_complex(std::complex<double>(re, im)));
}

pmt_t
from_complex(const std::complex<double> &z)
{
  return pmt_t(new pmt_complex(z));
}

std::complex<double>
to_complex(pmt_t x)
{
  if (x->is_complex())
    return _complex(x)->value();
  if (x->is_real())
    return _real(x)->value();
  if (x->is_integer())
    return _integer(x)->value();

  throw wrong_type("pmt_to_complex", x);
}

////////////////////////////////////////////////////////////////////////////
//                              Pairs
////////////////////////////////////////////////////////////////////////////

pmt_null::pmt_null() {}
pmt_pair::pmt_pair(const pmt_t& car, const pmt_t& cdr) : d_car(car), d_cdr(cdr) {}

bool
is_null(const pmt_t& x)
{
  return x == PMT_NIL;
}

bool
is_pair(const pmt_t& obj)
{
  return obj->is_pair();
}

pmt_t
cons(const pmt_t& x, const pmt_t& y)
{
  return pmt_t(new pmt_pair(x, y));
}

pmt_t
car(const pmt_t& pair)
{
  pmt_pair* p = dynamic_cast<pmt_pair*>(pair.get());
  if ( p )
    return p->car();

  throw wrong_type("pmt_car", pair);
}

pmt_t
cdr(const pmt_t& pair)
{
  pmt_pair* p = dynamic_cast<pmt_pair*>(pair.get());
  if ( p )
    return p->cdr();

  throw wrong_type("pmt_cdr", pair);
}

void
set_car(pmt_t pair, pmt_t obj)
{
  if (pair->is_pair())
    _pair(pair)->set_car(obj);
  else
    throw wrong_type("pmt_set_car", pair);
}

void
set_cdr(pmt_t pair, pmt_t obj)
{
  if (pair->is_pair())
    _pair(pair)->set_cdr(obj);
  else
    throw wrong_type("pmt_set_cdr", pair);
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
    throw out_of_range("pmt_vector_ref", from_long(k));
  return d_v[k];
}

void
pmt_vector::set(size_t k, pmt_t obj)
{
  if (k >= length())
    throw out_of_range("pmt_vector_set", from_long(k));
  d_v[k] = obj;
}

void
pmt_vector::fill(pmt_t obj)
{
  for (size_t i = 0; i < length(); i++)
    d_v[i] = obj;
}

bool
is_vector(pmt_t obj)
{
  return obj->is_vector();
}

pmt_t
make_vector(size_t k, pmt_t fill)
{
  return pmt_t(new pmt_vector(k, fill));
}

pmt_t
vector_ref(pmt_t vector, size_t k)
{
  if (!vector->is_vector())
    throw wrong_type("pmt_vector_ref", vector);
  return _vector(vector)->ref(k);
}

void
vector_set(pmt_t vector, size_t k, pmt_t obj)
{
  if (!vector->is_vector())
    throw wrong_type("pmt_vector_set", vector);
  _vector(vector)->set(k, obj);
}

void
vector_fill(pmt_t vector, pmt_t obj)
{
  if (!vector->is_vector())
    throw wrong_type("pmt_vector_set", vector);
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
    throw out_of_range("pmt_tuple_ref", from_long(k));
  return d_v[k];
}

bool
is_tuple(pmt_t obj)
{
  return obj->is_tuple();
}

pmt_t
tuple_ref(const pmt_t &tuple, size_t k)
{
  if (!tuple->is_tuple())
    throw wrong_type("pmt_tuple_ref", tuple);
  return _tuple(tuple)->ref(k);
}

// for (i=0; i < 10; i++)
//   make_constructor()

pmt_t
make_tuple()
{
  return pmt_t(new pmt_tuple(0));
}

pmt_t
make_tuple(const pmt_t &e0)
{
  pmt_tuple *t = new pmt_tuple(1);
  t->_set(0, e0);
  return pmt_t(t);
}

pmt_t
make_tuple(const pmt_t &e0, const pmt_t &e1)
{
  pmt_tuple *t = new pmt_tuple(2);
  t->_set(0, e0);
  t->_set(1, e1);
  return pmt_t(t);
}

pmt_t
make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2)
{
  pmt_tuple *t = new pmt_tuple(3);
  t->_set(0, e0);
  t->_set(1, e1);
  t->_set(2, e2);
  return pmt_t(t);
}

pmt_t
make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3)
{
  pmt_tuple *t = new pmt_tuple(4);
  t->_set(0, e0);
  t->_set(1, e1);
  t->_set(2, e2);
  t->_set(3, e3);
  return pmt_t(t);
}

pmt_t
make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4)
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
make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5)
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
make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6)
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
make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7)
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
make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7, const pmt_t &e8)
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
make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7, const pmt_t &e8, const pmt_t &e9)
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
to_tuple(const pmt_t &x)
{
  if (x->is_tuple())		// already one
    return x;

  size_t len = length(x);
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
      t->_set(i, car(y));
      y = cdr(y);
    }
    return r;
  }

  throw wrong_type("pmt_to_tuple", x);
}



////////////////////////////////////////////////////////////////////////////
//                       Uniform Numeric Vectors
////////////////////////////////////////////////////////////////////////////

bool
is_uniform_vector(pmt_t x)
{
  return x->is_uniform_vector();
}

size_t
uniform_vector_itemsize(pmt_t vector)
{
  if (!vector->is_uniform_vector())
    throw wrong_type("pmt_uniform_vector_itemsize", vector);
  return _uniform_vector(vector)->itemsize();
}

const void *
uniform_vector_elements(pmt_t vector, size_t &len)
{
  if (!vector->is_uniform_vector())
    throw wrong_type("pmt_uniform_vector_elements", vector);
  return _uniform_vector(vector)->uniform_elements(len);
}

void *
uniform_vector_writable_elements(pmt_t vector, size_t &len)
{
  if (!vector->is_uniform_vector())
    throw wrong_type("pmt_uniform_vector_writable_elements", vector);
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
is_dict(const pmt_t &obj)
{
  return is_null(obj) || is_pair(obj);
}

pmt_t
make_dict()
{
  return PMT_NIL;
}

pmt_t
dict_add(const pmt_t &dict, const pmt_t &key, const pmt_t &value)
{
  if (is_null(dict))
    return acons(key, value, PMT_NIL);

  if (dict_has_key(dict, key))
    return acons(key, value, dict_delete(dict, key));

  return acons(key, value, dict);
}

pmt_t
dict_update(const pmt_t &dict1, const pmt_t &dict2)
{
  pmt_t d(dict1);
  pmt_t k(dict_keys(dict2));
  while(is_pair(k)){
    d = dict_add(d, car(k), dict_ref(dict2, car(k), PMT_NIL));
    k = cdr(k);
    }
  return d;
}


pmt_t
dict_delete(const pmt_t &dict, const pmt_t &key)
{
  if (is_null(dict))
    return dict;

  if (eqv(caar(dict), key))
    return cdr(dict);

  return cons(car(dict), dict_delete(cdr(dict), key));
}

pmt_t
dict_ref(const pmt_t &dict, const pmt_t &key, const pmt_t &not_found)
{
  pmt_t	p = assv(key, dict);	// look for (key . value) pair
  if (is_pair(p))
    return cdr(p);
  else
    return not_found;
}

bool
dict_has_key(const pmt_t &dict, const pmt_t &key)
{
  return is_pair(assv(key, dict));
}

pmt_t
dict_items(pmt_t dict)
{
  if (!is_dict(dict))
    throw wrong_type("pmt_dict_values", dict);

  return dict;		// equivalent to dict in the a-list case
}

pmt_t
dict_keys(pmt_t dict)
{
  if (!is_dict(dict))
    throw wrong_type("pmt_dict_keys", dict);

  return map(car, dict);
}

pmt_t
dict_values(pmt_t dict)
{
  if (!is_dict(dict))
    throw wrong_type("pmt_dict_keys", dict);

  return map(cdr, dict);
}

////////////////////////////////////////////////////////////////////////////
//                                 Any
////////////////////////////////////////////////////////////////////////////

pmt_any::pmt_any(const boost::any &any) : d_any(any) {}

bool
is_any(pmt_t obj)
{
  return obj->is_any();
}

pmt_t
make_any(const boost::any &any)
{
  return pmt_t(new pmt_any(any));
}

boost::any
any_ref(pmt_t obj)
{
  if (!obj->is_any())
    throw wrong_type("pmt_any_ref", obj);
  return _any(obj)->ref();
}

void
any_set(pmt_t obj, const boost::any &any)
{
  if (!obj->is_any())
    throw wrong_type("pmt_any_set", obj);
  _any(obj)->set(any);
}

////////////////////////////////////////////////////////////////////////////
//               msg_accepter -- built from "any"
////////////////////////////////////////////////////////////////////////////

bool
is_msg_accepter(const pmt_t &obj)
{
  if (!is_any(obj))
    return false;

  boost::any r = any_ref(obj);
  return boost::any_cast<gr::messages::msg_accepter_sptr>(&r) != 0;
}

//! make a msg_accepter
pmt_t
make_msg_accepter(gr::messages::msg_accepter_sptr ma)
{
  return make_any(ma);
}

//! Return underlying msg_accepter
gr::messages::msg_accepter_sptr
msg_accepter_ref(const pmt_t &obj)
{
  try {
    return boost::any_cast<gr::messages::msg_accepter_sptr>(any_ref(obj));
  }
  catch (boost::bad_any_cast &e){
    throw wrong_type("pmt_msg_accepter_ref", obj);
  }
}


////////////////////////////////////////////////////////////////////////////
//             Binary Large Object -- currently a u8vector
////////////////////////////////////////////////////////////////////////////

bool
is_blob(pmt_t x)
{
  return is_u8vector(x);
}

pmt_t
make_blob(const void *buf, size_t len_in_bytes)
{
  return init_u8vector(len_in_bytes, (const uint8_t *) buf);
}

const void *
blob_data(pmt_t blob)
{
  size_t len;
  return uniform_vector_elements(blob, len);
}

size_t
blob_length(pmt_t blob)
{
  size_t len;
  uniform_vector_elements(blob, len);
  return len;
}


////////////////////////////////////////////////////////////////////////////
//                          General Functions
////////////////////////////////////////////////////////////////////////////

bool
eq(const pmt_t& x, const pmt_t& y)
{
  return x == y;
}

bool
eqv(const pmt_t& x, const pmt_t& y)
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
eqv_raw(pmt_base *x, pmt_base *y)
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
equal(const pmt_t& x, const pmt_t& y)
{
  if (eqv(x, y))
    return true;

  if (x->is_pair() && y->is_pair())
    return equal(car(x), car(y)) && equal(cdr(x), cdr(y));

  if (x->is_vector() && y->is_vector()){
    pmt_vector *xv = _vector(x);
    pmt_vector *yv = _vector(y);
    if (xv->length() != yv->length())
      return false;

    for (unsigned i = 0; i < xv->length(); i++)
      if (!equal(xv->_ref(i), yv->_ref(i)))
	return false;

    return true;
  }

  if (x->is_tuple() && y->is_tuple()){
    pmt_tuple *xv = _tuple(x);
    pmt_tuple *yv = _tuple(y);
    if (xv->length() != yv->length())
      return false;

    for (unsigned i = 0; i < xv->length(); i++)
      if (!equal(xv->_ref(i), yv->_ref(i)))
	return false;

    return true;
  }

  if (x->is_uniform_vector() && y->is_uniform_vector()){
    pmt_uniform_vector *xv = _uniform_vector(x);
    pmt_uniform_vector *yv = _uniform_vector(y);
    if (xv->length() != yv->length())
      return false;

    size_t len_x, len_y;
    const void *x_m = xv->uniform_elements(len_x);
    const void *y_m = yv->uniform_elements(len_y);
    if (memcmp(x_m, y_m, len_x) == 0)
      return true;

    return false;
  }

  // FIXME add other cases here...

  return false;
}

size_t
length(const pmt_t& x)
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
    pmt_t it = cdr(x);
    while (is_pair(it)){
      length++;
      it = cdr(it);
    }
    if (is_null(it))
      return length;

    // not a proper list
    throw wrong_type("pmt_length", x);
  }

  // FIXME dictionary length (number of entries)

  throw wrong_type("pmt_length", x);
}

pmt_t
assq(pmt_t obj, pmt_t alist)
{
  while (is_pair(alist)){
    pmt_t p = car(alist);
    if (!is_pair(p))	// malformed alist
      return PMT_F;

    if (eq(obj, car(p)))
      return p;

    alist = cdr(alist);
  }
  return PMT_F;
}

/*
 * This avoids a bunch of shared_pointer reference count manipulation.
 */
pmt_t
assv_raw(pmt_base *obj, pmt_base *alist)
{
  while (alist->is_pair()){
    pmt_base *p = ((pmt_pair *)alist)->d_car.get();
    if (!p->is_pair())		// malformed alist
      return PMT_F;

    if (eqv_raw(obj, ((pmt_pair *)p)->d_car.get()))
      return ((pmt_pair *)alist)->d_car;

    alist = (((pmt_pair *)alist)->d_cdr).get();
  }
  return PMT_F;
}

#if 1

pmt_t
assv(pmt_t obj, pmt_t alist)
{
  return assv_raw(obj.get(), alist.get());
}

#else

pmt_t
assv(pmt_t obj, pmt_t alist)
{
  while (is_pair(alist)){
    pmt_t p = car(alist);
    if (!is_pair(p))	// malformed alist
      return PMT_F;

    if (eqv(obj, car(p)))
      return p;

    alist = cdr(alist);
  }
  return PMT_F;
}

#endif


pmt_t
assoc(pmt_t obj, pmt_t alist)
{
  while (is_pair(alist)){
    pmt_t p = car(alist);
    if (!is_pair(p))	// malformed alist
      return PMT_F;

    if (equal(obj, car(p)))
      return p;

    alist = cdr(alist);
  }
  return PMT_F;
}

pmt_t
map(pmt_t proc(const pmt_t&), pmt_t list)
{
  pmt_t r = PMT_NIL;

  while(is_pair(list)){
    r = cons(proc(car(list)), r);
    list = cdr(list);
  }

  return reverse_x(r);
}

pmt_t
reverse(pmt_t listx)
{
  pmt_t list = listx;
  pmt_t r = PMT_NIL;

  while(is_pair(list)){
    r = cons(car(list), r);
    list = cdr(list);
  }
  if (is_null(list))
    return r;
  else
    throw wrong_type("pmt_reverse", listx);
}

pmt_t
reverse_x(pmt_t list)
{
  // FIXME do it destructively
  return reverse(list);
}

pmt_t
nth(size_t n, pmt_t list)
{
  pmt_t t = nthcdr(n, list);
  if (is_pair(t))
    return car(t);
  else
    return PMT_NIL;
}

pmt_t
nthcdr(size_t n, pmt_t list)
{
  if (!(is_pair(list) || is_null(list)))
    throw wrong_type("pmt_nthcdr", list);

  while (n > 0){
    if (is_pair(list)){
      list = cdr(list);
      n--;
      continue;
    }
    if (is_null(list))
      return PMT_NIL;
    else
      throw wrong_type("pmt_nthcdr: not a LIST", list);
  }
  return list;
}

pmt_t
memq(pmt_t obj, pmt_t list)
{
  while (is_pair(list)){
    if (eq(obj, car(list)))
      return list;
    list = cdr(list);
  }
  return PMT_F;
}

pmt_t
memv(pmt_t obj, pmt_t list)
{
  while (is_pair(list)){
    if (eqv(obj, car(list)))
      return list;
    list = cdr(list);
  }
  return PMT_F;
}

pmt_t
member(pmt_t obj, pmt_t list)
{
  while (is_pair(list)){
    if (equal(obj, car(list)))
      return list;
    list = cdr(list);
  }
  return PMT_F;
}

bool
subsetp(pmt_t list1, pmt_t list2)
{
  while (is_pair(list1)){
    pmt_t p = car(list1);
    if (is_false(memv(p, list2)))
      return false;
    list1 = cdr(list1);
  }
  return true;
}

pmt_t
list1(const pmt_t& x1)
{
  return cons(x1, PMT_NIL);
}

pmt_t
list2(const pmt_t& x1, const pmt_t& x2)
{
  return cons(x1, cons(x2, PMT_NIL));
}

pmt_t
list3(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3)
{
  return cons(x1, cons(x2, cons(x3, PMT_NIL)));
}

pmt_t
list4(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4)
{
  return cons(x1, cons(x2, cons(x3, cons(x4, PMT_NIL))));
}

pmt_t
list5(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4, const pmt_t& x5)
{
  return cons(x1, cons(x2, cons(x3, cons(x4, cons(x5, PMT_NIL)))));
}

pmt_t
list6(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4, const pmt_t& x5, const pmt_t& x6)
{
  return cons(x1, cons(x2, cons(x3, cons(x4, cons(x5, cons(x6, PMT_NIL))))));
}

pmt_t
list_add(pmt_t list, const pmt_t& item)
{
  return reverse(cons(item, reverse(list)));
}

pmt_t
list_rm(pmt_t list, const pmt_t& item)
{
  if(is_pair(list)){
    pmt_t left = car(list);
    pmt_t right = cdr(list);
    if(!equal(left, item)){
        return cons(left, list_rm(right, item));
        } else {
        return list_rm(right, item);
        }
    } else {
      return list;
    }
}

bool
list_has(pmt_t list, const pmt_t& item)
{
  if(is_pair(list)){
    pmt_t left = car(list);
    pmt_t right = cdr(list);
    if(equal(left,item))
        return true;
    return list_has(right, item);
  } else {
    if(is_null(list))
        return false;
    throw std::runtime_error("list contains invalid format!");
  }
}

pmt_t
caar(pmt_t pair)
{
  return (car(car(pair)));
}

pmt_t
cadr(pmt_t pair)
{
  return car(cdr(pair));
}

pmt_t
cdar(pmt_t pair)
{
  return cdr(car(pair));
}

pmt_t
cddr(pmt_t pair)
{
  return cdr(cdr(pair));
}

pmt_t
caddr(pmt_t pair)
{
  return car(cdr(cdr(pair)));
}

pmt_t
cadddr(pmt_t pair)
{
  return car(cdr(cdr(cdr(pair))));
}

bool
is_eof_object(pmt_t obj)
{
  return eq(obj, PMT_EOF);
}

void
dump_sizeof()
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

} /* namespace pmt */

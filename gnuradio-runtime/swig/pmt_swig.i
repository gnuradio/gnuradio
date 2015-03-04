/* -*- c++ -*- */
/*
 * Copyright 2011-2013 Free Software Foundation, Inc.
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

#pragma SWIG nowarn=401

%include "std_string.i"
%include "stdint.i"

%{
#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <complex>
#include <string>
#include <stddef.h>		// size_t
#include <stdint.h>
#include <iosfwd>
#include <stdexcept>
#include <pmt/pmt.h>
%}

%feature("autodoc","1");

//load generated python docstrings
%include "pmt_swig_doc.i"

%include <gr_extras.i>

%include <std_complex.i>
%include <std_vector.i>
%template(pmt_vector_int8) std::vector<int8_t>;
%template(pmt_vector_uint8) std::vector<uint8_t>;
%template(pmt_vector_int16) std::vector<int16_t>;
%template(pmt_vector_uint16) std::vector<uint16_t>;
%template(pmt_vector_int32) std::vector<int32_t>;
%template(pmt_vector_uint32) std::vector<uint32_t>;
%template(pmt_vector_float) std::vector<float>;
%template(pmt_vector_double) std::vector<double>;
%template(pmt_vector_cfloat) std::vector< std::complex<float> >;
%template(pmt_vector_cdouble) std::vector< std::complex<double> >;

////////////////////////////////////////////////////////////////////////
// Language independent exception handler
////////////////////////////////////////////////////////////////////////

// Template intrusive_ptr for Swig to avoid dereferencing issues
namespace pmt{
    class pmt_base;
}
//%import <intrusive_ptr.i>
%import <gr_intrusive_ptr.i>
%template(swig_int_ptr) boost::intrusive_ptr<pmt::pmt_base>;

namespace pmt{

  typedef boost::intrusive_ptr<pmt_base> pmt_t;

  // Allows Python to directly print a PMT object
  %pythoncode
  %{
    swig_int_ptr.__repr__ = lambda self: write_string(self)
  %}

  pmt_t get_PMT_NIL();
  pmt_t get_PMT_T();
  pmt_t get_PMT_F();
  pmt_t get_PMT_EOF();

  #define PMT_NIL get_PMT_NIL()
  #define PMT_T get_PMT_T()
  #define PMT_F get_PMT_F()
  #define PMT_EOF get_PMT_EOF()

  bool is_bool(pmt_t obj);
  bool is_true(pmt_t obj);
  bool is_false(pmt_t obj);
  pmt_t from_bool(bool val);
  bool to_bool(pmt_t val);

  bool is_symbol(const pmt_t& obj);
  pmt_t string_to_symbol(const std::string &s);
  pmt_t intern(const std::string &s);
  const std::string symbol_to_string(const pmt_t& sym);

  bool is_number(pmt_t obj);
  bool is_integer(pmt_t x);
  pmt_t from_long(long x);
  long to_long(pmt_t x);

  bool is_uint64(pmt_t x);
  pmt_t from_uint64(uint64_t x);
  uint64_t to_uint64(pmt_t x);

  bool is_real(pmt_t obj);
  pmt_t from_double(double x);
  double to_double(pmt_t x);
  pmt_t from_float(double x);
  double to_float(pmt_t x);

  bool is_complex(pmt_t obj);
  pmt_t make_rectangular(double re, double im);
  pmt_t from_complex(const std::complex<double> &z);
  std::complex<double> to_complex(pmt_t z);

  bool is_null(const pmt_t& x);
  bool is_pair(const pmt_t& obj);
  pmt_t cons(const pmt_t& x, const pmt_t& y);
  pmt_t car(const pmt_t& pair);
  pmt_t cdr(const pmt_t& pair);
  void set_car(pmt_t pair, pmt_t value);
  void set_cdr(pmt_t pair, pmt_t value);

  pmt_t caar(pmt_t pair);
  pmt_t cadr(pmt_t pair);
  pmt_t cdar(pmt_t pair);
  pmt_t cddr(pmt_t pair);
  pmt_t caddr(pmt_t pair);
  pmt_t cadddr(pmt_t pair);

  bool is_tuple(pmt_t x);
  pmt_t make_tuple();
  pmt_t make_tuple(const pmt_t &e0);
  pmt_t make_tuple(const pmt_t &e0, const pmt_t &e1);
  pmt_t make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2);
  pmt_t make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3);
  pmt_t make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4);
  pmt_t make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5);
  pmt_t make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6);
  pmt_t make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7);
  pmt_t make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7, const pmt_t &e8);
  pmt_t make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7, const pmt_t &e8, const pmt_t &e9);

  pmt_t to_tuple(const pmt_t &x);
  pmt_t tuple_ref(const pmt_t &tuple, size_t k);

  bool is_vector(pmt_t x);
  pmt_t make_vector(size_t k, pmt_t fill);
  pmt_t vector_ref(pmt_t vector, size_t k);
  void vector_set(pmt_t vector, size_t k, pmt_t obj);
  void vector_fill(pmt_t vector, pmt_t fill);

  bool is_blob(pmt_t x);
  pmt_t make_blob(const void *buf, size_t len);
  const void *blob_data(pmt_t blob);
  size_t blob_length(pmt_t blob);

  bool is_uniform_vector(pmt_t x);
  bool is_u8vector(pmt_t x);
  bool is_s8vector(pmt_t x);
  bool is_u16vector(pmt_t x);
  bool is_s16vector(pmt_t x);
  bool is_u32vector(pmt_t x);
  bool is_s32vector(pmt_t x);
  bool is_u64vector(pmt_t x);
  bool is_s64vector(pmt_t x);
  bool is_f32vector(pmt_t x);
  bool is_f64vector(pmt_t x);
  bool is_c32vector(pmt_t x);
  bool is_c64vector(pmt_t x);
  size_t uniform_vector_itemsize(pmt_t x);
  pmt_t make_u8vector(size_t k, uint8_t fill);
  pmt_t make_s8vector(size_t k, int8_t fill);
  pmt_t make_u16vector(size_t k, uint16_t fill);
  pmt_t make_s16vector(size_t k, int16_t fill);
  pmt_t make_u32vector(size_t k, uint32_t fill);
  pmt_t make_s32vector(size_t k, int32_t fill);
  pmt_t make_u64vector(size_t k, uint64_t fill);
  pmt_t make_s64vector(size_t k, int64_t fill);
  pmt_t make_f32vector(size_t k, float fill);
  pmt_t make_f64vector(size_t k, double fill);
  pmt_t make_c32vector(size_t k, std::complex<float> fill);
  pmt_t make_c64vector(size_t k, std::complex<double> fill);
  pmt_t init_u8vector(size_t k, const std::vector<uint8_t> &data);
  pmt_t init_s8vector(size_t k, const std::vector<int8_t> &data);
  pmt_t init_u16vector(size_t k, const std::vector<uint16_t> &data);
  pmt_t init_s16vector(size_t k, const std::vector<int16_t> &data);
  pmt_t init_u32vector(size_t k, const std::vector<uint32_t> &data);
  pmt_t init_s32vector(size_t k, const std::vector<int32_t> &data);
  pmt_t init_f32vector(size_t k, const std::vector<float> &data);
  pmt_t init_f64vector(size_t k, const std::vector<double> &data);
  pmt_t init_c32vector(size_t k, const std::vector<std::complex<float> > &data);
  pmt_t init_c64vector(size_t k, const std::vector<std::complex<double> > &data);
  uint8_t  u8vector_ref(pmt_t v, size_t k);
  int8_t   s8vector_ref(pmt_t v, size_t k);
  uint16_t u16vector_ref(pmt_t v, size_t k);
  int16_t  s16vector_ref(pmt_t v, size_t k);
  uint32_t u32vector_ref(pmt_t v, size_t k);
  int32_t  s32vector_ref(pmt_t v, size_t k);
  uint64_t u64vector_ref(pmt_t v, size_t k);
  int64_t  s64vector_ref(pmt_t v, size_t k);
  float    f32vector_ref(pmt_t v, size_t k);
  double   f64vector_ref(pmt_t v, size_t k);
  std::complex<float>  c32vector_ref(pmt_t v, size_t k);
  std::complex<double> c64vector_ref(pmt_t v, size_t k);
  void u8vector_set(pmt_t v, size_t k, uint8_t x);  //< v[k] = x
  void s8vector_set(pmt_t v, size_t k, int8_t x);
  void u16vector_set(pmt_t v, size_t k, uint16_t x);
  void s16vector_set(pmt_t v, size_t k, int16_t x);
  void u32vector_set(pmt_t v, size_t k, uint32_t x);
  void s32vector_set(pmt_t v, size_t k, int32_t x);
  void u64vector_set(pmt_t v, size_t k, uint64_t x);
  void s64vector_set(pmt_t v, size_t k, int64_t x);
  void f32vector_set(pmt_t v, size_t k, float x);
  void f64vector_set(pmt_t v, size_t k, double x);
  void c32vector_set(pmt_t v, size_t k, std::complex<float> x);
  void c64vector_set(pmt_t v, size_t k, std::complex<double> x);

  %apply size_t & INOUT { size_t &len };
  const void *uniform_vector_elements(pmt_t v, size_t &len);

  const std::vector<uint8_t>  u8vector_elements(pmt_t v);
  const std::vector<int8_t>   s8vector_elements(pmt_t v);
  const std::vector<uint16_t> u16vector_elements(pmt_t v);
  const std::vector<int16_t>  s16vector_elements(pmt_t v);
  const std::vector<uint32_t> u32vector_elements(pmt_t v);
  const std::vector<int32_t>  s32vector_elements(pmt_t v);
  const std::vector<float>    f32vector_elements(pmt_t v);
  const std::vector<double>   f64vector_elements(pmt_t v);
  const std::vector<std::complex<float> > c32vector_elements(pmt_t v);
  const std::vector<std::complex<double> > c64vector_elements(pmt_t v);

  bool is_dict(const pmt_t &obj);
  pmt_t make_dict();
  pmt_t dict_add(const pmt_t &dict, const pmt_t &key, const pmt_t &value);
  pmt_t dict_delete(const pmt_t &dict, const pmt_t &key);
  bool  dict_has_key(const pmt_t &dict, const pmt_t &key);
  pmt_t dict_ref(const pmt_t &dict, const pmt_t &key, const pmt_t &not_found);
  pmt_t dict_items(pmt_t dict);
  pmt_t dict_keys(pmt_t dict);
  pmt_t dict_update(const pmt_t &dict1, const pmt_t &dict2);
  pmt_t dict_values(pmt_t dict);

  bool is_any(pmt_t obj);
  pmt_t make_any(const boost::any &any);
  boost::any any_ref(pmt_t obj);
  void any_set(pmt_t obj, const boost::any &any);

  bool is_msg_accepter(const pmt_t &obj);
  pmt_t make_msg_accepter(boost::shared_ptr<gr::messages::msg_accepter> ma);
  boost::shared_ptr<gr::messages::msg_accepter> msg_accepter_ref(const pmt_t &obj);

  bool eq(const pmt_t& x, const pmt_t& y);
  bool eqv(const pmt_t& x, const pmt_t& y);
  bool equal(const pmt_t& x, const pmt_t& y);
  size_t length(const pmt_t& v);
  pmt_t assq(pmt_t obj, pmt_t alist);
  pmt_t assv(pmt_t obj, pmt_t alist);
  pmt_t assoc(pmt_t obj, pmt_t alist);
  pmt_t map(pmt_t proc(const pmt_t&), pmt_t list);
  pmt_t reverse(pmt_t list);
  pmt_t reverse_x(pmt_t list);
  inline static pmt_t acons(pmt_t x, pmt_t y, pmt_t a);
  pmt_t nth(size_t n, pmt_t list);
  pmt_t nthcdr(size_t n, pmt_t list);
  pmt_t memq(pmt_t obj, pmt_t list);
  pmt_t memv(pmt_t obj, pmt_t list);
  pmt_t member(pmt_t obj, pmt_t list);
  bool subsetp(pmt_t list1, pmt_t list2);

  pmt_t list1(const pmt_t& x1);
  pmt_t list2(const pmt_t& x1, const pmt_t& x2);
  pmt_t list3(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3);
  pmt_t list4(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4);
  pmt_t list5(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4, const pmt_t& x5);
  pmt_t list6(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4, const pmt_t& x5, const pmt_t& x6);
  pmt_t list_add(pmt_t list, const pmt_t& item);
  pmt_t list_rm(pmt_t list, const pmt_t& item);
  bool list_has(pmt_t list, const pmt_t& item);

  bool is_eof_object(pmt_t obj);
  pmt_t read(std::istream &port);
  void write(pmt_t obj, std::ostream &port);
  std::string write_string(pmt_t obj);

  //void pmt_print(pmt_t v);

  bool serialize(pmt_t obj, std::streambuf &sink);
  pmt_t deserialize(std::streambuf &source);
  void dump_sizeof();
  std::string serialize_str(pmt_t obj);
  pmt_t deserialize_str(std::string str);

} //namespace pmt

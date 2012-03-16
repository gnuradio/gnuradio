/* -*- c++ -*- */
/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
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

%include "std_string.i"
%include "stdint.i"
%{
#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <complex>
#include <string>
#include <stdint.h>
#include <iosfwd>
#include <stdexcept>
#include <gruel/pmt.h>
%}

%include "gruel_common.i"
//load generated python docstrings
%include "pmt_swig_doc.i"

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

/*
 * ------------------------------------------------------------------------
 * Booleans.  Two constants, #t and #f.
 *
 * In predicates, anything that is not #f is considered true.
 * I.e., there is a single false value, #f.
 * ------------------------------------------------------------------------
 */
extern const pmt_t PMT_T;
extern const pmt_t PMT_F;

//! Return true if obj is \#t or \#f, else return false.
bool pmt_is_bool(pmt_t obj);

//! Return false if obj is \#f, else return true.
bool pmt_is_true(pmt_t obj);

//! Return true if obj is \#f, else return true.
bool pmt_is_false(pmt_t obj);

//! Return \#f is val is false, else return \#t.
pmt_t pmt_from_bool(bool val);

//! Return true if val is PMT_T, return false when val is PMT_F, 
// else raise wrong_type exception.
bool pmt_to_bool(pmt_t val);

/*
 * ------------------------------------------------------------------------
 *			       Symbols
 * ------------------------------------------------------------------------
 */

//! Return true if obj is a symbol, else false.
bool pmt_is_symbol(const pmt_t& obj);

//! Return the symbol whose name is \p s.
pmt_t pmt_string_to_symbol(const std::string &s);

//! Alias for pmt_string_to_symbol
pmt_t pmt_intern(const std::string &s);


/*!
 * If \p is a symbol, return the name of the symbol as a string.
 * Otherwise, raise the wrong_type exception.
 */
const std::string pmt_symbol_to_string(const pmt_t& sym);

/*
 * ------------------------------------------------------------------------
 *           Numbers: we support integer, real and complex
 * ------------------------------------------------------------------------
 */

//! Return true if obj is any kind of number, else false.
bool pmt_is_number(pmt_t obj);

/*
 * ------------------------------------------------------------------------
 *			       Integers
 * ------------------------------------------------------------------------
 */

//! Return true if \p x is an integer number, else false
bool pmt_is_integer(pmt_t x);

//! Return the pmt value that represents the integer \p x.
pmt_t pmt_from_long(long x);

/*!
 * \brief Convert pmt to long if possible.
 *
 * When \p x represents an exact integer that fits in a long,
 * return that integer.  Else raise an exception, either wrong_type
 * when x is not an exact integer, or out_of_range when it doesn't fit.
 */
long pmt_to_long(pmt_t x);

/*
 * ------------------------------------------------------------------------
 *			       uint64_t
 * ------------------------------------------------------------------------
 */

//! Return true if \p x is an uint64 number, else false
bool pmt_is_uint64(pmt_t x);

//! Return the pmt value that represents the uint64 \p x.
pmt_t pmt_from_uint64(uint64_t x);

/*!
 * \brief Convert pmt to uint64 if possible.
 *
 * When \p x represents an exact integer that fits in a uint64,
 * return that uint64.  Else raise an exception, either wrong_type
 * when x is not an exact uint64, or out_of_range when it doesn't fit.
 */
uint64_t pmt_to_uint64(pmt_t x);

/*
 * ------------------------------------------------------------------------
 *				Reals
 * ------------------------------------------------------------------------
 */

/*
 * \brief Return true if \p obj is a real number, else false.
 */
bool pmt_is_real(pmt_t obj);

//! Return the pmt value that represents double \p x.
pmt_t pmt_from_double(double x);

/*!
 * \brief Convert pmt to double if possible.
 *
 * Returns the number closest to \p val that is representable
 * as a double.  The argument \p val must be a real or integer, otherwise
 * a wrong_type exception is raised.
 */
double pmt_to_double(pmt_t x);

/*
 * ------------------------------------------------------------------------
 *			       Complex
 * ------------------------------------------------------------------------
 */

/*!
 * \brief return true if \p obj is a complex number, false otherwise.
 */
bool pmt_is_complex(pmt_t obj);

//! Return a complex number constructed of the given real and imaginary parts.
pmt_t pmt_make_rectangular(double re, double im);

/*!
 * If \p z is complex, real or integer, return the closest complex<double>.
 * Otherwise, raise the wrong_type exception.
 */
std::complex<double> pmt_to_complex(pmt_t z);

/*
 * ------------------------------------------------------------------------
 *				Pairs
 * ------------------------------------------------------------------------
 */

extern const pmt_t PMT_NIL;	//< the empty list

//! Return true if \p x is the empty list, otherwise return false.
bool pmt_is_null(const pmt_t& x);

//! Return true if \p obj is a pair, else false.
bool pmt_is_pair(const pmt_t& obj);

//! Return a newly allocated pair whose car is \p x and whose cdr is \p y.
pmt_t pmt_cons(const pmt_t& x, const pmt_t& y);

//! If \p pair is a pair, return the car of the \p pair, otherwise raise wrong_type.
pmt_t pmt_car(const pmt_t& pair);

//! If \p pair is a pair, return the cdr of the \p pair, otherwise raise wrong_type.
pmt_t pmt_cdr(const pmt_t& pair);

//! Stores \p value in the car field of \p pair.
void pmt_set_car(pmt_t pair, pmt_t value);

//! Stores \p value in the cdr field of \p pair.
void pmt_set_cdr(pmt_t pair, pmt_t value);

pmt_t pmt_caar(pmt_t pair);
pmt_t pmt_cadr(pmt_t pair);
pmt_t pmt_cdar(pmt_t pair);
pmt_t pmt_cddr(pmt_t pair);
pmt_t pmt_caddr(pmt_t pair);
pmt_t pmt_cadddr(pmt_t pair);

/*
 * ------------------------------------------------------------------------
 *			          Tuples
 *
 * Store a fixed number of objects.  Tuples are not modifiable, and thus
 * are excellent for use as messages.  Indexing is zero based.
 * Access time to an element is O(1).
 * ------------------------------------------------------------------------
 */

//! Return true if \p x is a tuple, othewise false.
bool pmt_is_tuple(pmt_t x);

pmt_t pmt_make_tuple();
pmt_t pmt_make_tuple(const pmt_t &e0);
pmt_t pmt_make_tuple(const pmt_t &e0, const pmt_t &e1);
pmt_t pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2);
pmt_t pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3);
pmt_t pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4);
pmt_t pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5);
pmt_t pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6);
pmt_t pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7);
pmt_t pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7, const pmt_t &e8);
pmt_t pmt_make_tuple(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7, const pmt_t &e8, const pmt_t &e9);

/*!
 * If \p x is a vector or proper list, return a tuple containing the elements of x
 */
pmt_t pmt_to_tuple(const pmt_t &x);

/*!
 * Return the contents of position \p k of \p tuple.
 * \p k must be a valid index of \p tuple.
 */
pmt_t pmt_tuple_ref(const pmt_t &tuple, size_t k);

/*
 * ------------------------------------------------------------------------
 *			       Vectors
 *
 * These vectors can hold any kind of objects.  Indexing is zero based.
 * ------------------------------------------------------------------------
 */

//! Return true if \p x is a vector, othewise false.
bool pmt_is_vector(pmt_t x);

//! Make a vector of length \p k, with initial values set to \p fill
pmt_t pmt_make_vector(size_t k, pmt_t fill);

/*!
 * Return the contents of position \p k of \p vector.
 * \p k must be a valid index of \p vector.
 */
pmt_t pmt_vector_ref(pmt_t vector, size_t k);

//! Store \p obj in position \p k.
void pmt_vector_set(pmt_t vector, size_t k, pmt_t obj);

//! Store \p fill in every position of \p vector
void pmt_vector_fill(pmt_t vector, pmt_t fill);

/*
 * ------------------------------------------------------------------------
 *		      Binary Large Objects (BLOBs)
 *
 * Handy for passing around uninterpreted chunks of memory.
 * ------------------------------------------------------------------------
 */

//! Return true if \p x is a blob, othewise false.
bool pmt_is_blob(pmt_t x);

/*!
 * \brief Make a blob given a pointer and length in bytes
 *
 * \param buf is the pointer to data to use to create blob
 * \param len is the size of the data in bytes.
 *
 * The data is copied into the blob.
 */
pmt_t pmt_make_blob(const void *buf, size_t len);

//! Return a pointer to the blob's data
const void *pmt_blob_data(pmt_t blob);

//! Return the blob's length in bytes
size_t pmt_blob_length(pmt_t blob);

/*!
 * <pre>
 * ------------------------------------------------------------------------
 *		       Uniform Numeric Vectors
 *
 * A uniform numeric vector is a vector whose elements are all of single
 * numeric type.  pmt offers uniform numeric vectors for signed and
 * unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers, two sizes of
 * floating point values, and complex floating-point numbers of these
 * two sizes.  Indexing is zero based.
 *
 * The names of the functions include these tags in their names:
 *
 *    u8  unsigned 8-bit integers
 *    s8  signed 8-bit integers
 *   u16  unsigned 16-bit integers
 *   s16  signed 16-bit integers
 *   u32  unsigned 32-bit integers
 *   s32  signed 32-bit integers
 *   u64  unsigned 64-bit integers
 *   s64  signed 64-bit integers
 *   f32  the C++ type float
 *   f64  the C++ type double
 *   c32  the C++ type complex<float>
 *   c64  the C++ type complex<double>
 * ------------------------------------------------------------------------
 * </pre>
 */

//! true if \p x is any kind of uniform numeric vector
bool pmt_is_uniform_vector(pmt_t x);  

bool pmt_is_u8vector(pmt_t x);
bool pmt_is_s8vector(pmt_t x);
bool pmt_is_u16vector(pmt_t x);
bool pmt_is_s16vector(pmt_t x);
bool pmt_is_u32vector(pmt_t x);
bool pmt_is_s32vector(pmt_t x);
bool pmt_is_u64vector(pmt_t x);
bool pmt_is_s64vector(pmt_t x);
bool pmt_is_f32vector(pmt_t x);
bool pmt_is_f64vector(pmt_t x);
bool pmt_is_c32vector(pmt_t x);
bool pmt_is_c64vector(pmt_t x);

pmt_t pmt_make_u8vector(size_t k, uint8_t fill);
pmt_t pmt_make_s8vector(size_t k, int8_t fill);
pmt_t pmt_make_u16vector(size_t k, uint16_t fill);
pmt_t pmt_make_s16vector(size_t k, int16_t fill);
pmt_t pmt_make_u32vector(size_t k, uint32_t fill);
pmt_t pmt_make_s32vector(size_t k, int32_t fill);
pmt_t pmt_make_u64vector(size_t k, uint64_t fill);
pmt_t pmt_make_s64vector(size_t k, int64_t fill);
pmt_t pmt_make_f32vector(size_t k, float fill);
pmt_t pmt_make_f64vector(size_t k, double fill);
pmt_t pmt_make_c32vector(size_t k, std::complex<float> fill);
pmt_t pmt_make_c64vector(size_t k, std::complex<double> fill);

pmt_t pmt_init_u8vector(size_t k, const uint8_t *data);
pmt_t pmt_init_s8vector(size_t k, const int8_t *data);
pmt_t pmt_init_u16vector(size_t k, const uint16_t *data);
pmt_t pmt_init_s16vector(size_t k, const int16_t *data);
pmt_t pmt_init_u32vector(size_t k, const uint32_t *data);
pmt_t pmt_init_s32vector(size_t k, const int32_t *data);
pmt_t pmt_init_f32vector(size_t k, const float *data);
pmt_t pmt_init_f64vector(size_t k, const double *data);
pmt_t pmt_init_c32vector(size_t k, const std::complex<float> *data);
pmt_t pmt_init_c64vector(size_t k, const std::complex<double> *data);

uint8_t  pmt_u8vector_ref(pmt_t v, size_t k);
int8_t   pmt_s8vector_ref(pmt_t v, size_t k);
uint16_t pmt_u16vector_ref(pmt_t v, size_t k);
int16_t  pmt_s16vector_ref(pmt_t v, size_t k);
uint32_t pmt_u32vector_ref(pmt_t v, size_t k);
int32_t  pmt_s32vector_ref(pmt_t v, size_t k);
uint64_t pmt_u64vector_ref(pmt_t v, size_t k);
int64_t  pmt_s64vector_ref(pmt_t v, size_t k);
float    pmt_f32vector_ref(pmt_t v, size_t k);
double   pmt_f64vector_ref(pmt_t v, size_t k);
std::complex<float>  pmt_c32vector_ref(pmt_t v, size_t k);
std::complex<double> pmt_c64vector_ref(pmt_t v, size_t k);

void pmt_u8vector_set(pmt_t v, size_t k, uint8_t x);  //< v[k] = x
void pmt_s8vector_set(pmt_t v, size_t k, int8_t x);
void pmt_u16vector_set(pmt_t v, size_t k, uint16_t x);
void pmt_s16vector_set(pmt_t v, size_t k, int16_t x);
void pmt_u32vector_set(pmt_t v, size_t k, uint32_t x);
void pmt_s32vector_set(pmt_t v, size_t k, int32_t x);
void pmt_u64vector_set(pmt_t v, size_t k, uint64_t x);
void pmt_s64vector_set(pmt_t v, size_t k, int64_t x);
void pmt_f32vector_set(pmt_t v, size_t k, float x);
void pmt_f64vector_set(pmt_t v, size_t k, double x);
void pmt_c32vector_set(pmt_t v, size_t k, std::complex<float> x);
void pmt_c64vector_set(pmt_t v, size_t k, std::complex<double> x);

// Return const pointers to the elements

const void *pmt_uniform_vector_elements(pmt_t v, size_t &len);  //< works with any; len is in bytes

const uint8_t  *pmt_u8vector_elements(pmt_t v, size_t &len);  //< len is in elements
const int8_t   *pmt_s8vector_elements(pmt_t v, size_t &len);  //< len is in elements
const uint16_t *pmt_u16vector_elements(pmt_t v, size_t &len); //< len is in elements
const int16_t  *pmt_s16vector_elements(pmt_t v, size_t &len); //< len is in elements
const uint32_t *pmt_u32vector_elements(pmt_t v, size_t &len); //< len is in elements
const int32_t  *pmt_s32vector_elements(pmt_t v, size_t &len); //< len is in elements
const uint64_t *pmt_u64vector_elements(pmt_t v, size_t &len); //< len is in elements
const int64_t  *pmt_s64vector_elements(pmt_t v, size_t &len); //< len is in elements
const float    *pmt_f32vector_elements(pmt_t v, size_t &len); //< len is in elements
const double   *pmt_f64vector_elements(pmt_t v, size_t &len); //< len is in elements
const std::complex<float>  *pmt_c32vector_elements(pmt_t v, size_t &len); //< len is in elements
const std::complex<double> *pmt_c64vector_elements(pmt_t v, size_t &len); //< len is in elements

// Return non-const pointers to the elements

void *pmt_uniform_vector_writable_elements(pmt_t v, size_t &len);  //< works with any; len is in bytes

uint8_t  *pmt_u8vector_writable_elements(pmt_t v, size_t &len);  //< len is in elements
int8_t   *pmt_s8vector_writable_elements(pmt_t v, size_t &len);  //< len is in elements
uint16_t *pmt_u16vector_writable_elements(pmt_t v, size_t &len); //< len is in elements
int16_t  *pmt_s16vector_writable_elements(pmt_t v, size_t &len); //< len is in elements
uint32_t *pmt_u32vector_writable_elements(pmt_t v, size_t &len); //< len is in elements
int32_t  *pmt_s32vector_writable_elements(pmt_t v, size_t &len); //< len is in elements
uint64_t *pmt_u64vector_writable_elements(pmt_t v, size_t &len); //< len is in elements
int64_t  *pmt_s64vector_writable_elements(pmt_t v, size_t &len); //< len is in elements
float    *pmt_f32vector_writable_elements(pmt_t v, size_t &len); //< len is in elements
double   *pmt_f64vector_writable_elements(pmt_t v, size_t &len); //< len is in elements
std::complex<float>  *pmt_c32vector_writable_elements(pmt_t v, size_t &len); //< len is in elements
std::complex<double> *pmt_c64vector_writable_elements(pmt_t v, size_t &len); //< len is in elements

/*
 * ------------------------------------------------------------------------
 *	   Dictionary (a.k.a associative array, hash, map)
 *
 * This is a functional data structure that is persistent.  Updating a
 * functional data structure does not destroy the existing version, but
 * rather creates a new version that coexists with the old.
 * ------------------------------------------------------------------------
 */

//! Return true if \p obj is a dictionary
bool pmt_is_dict(const pmt_t &obj);

//! Make an empty dictionary
pmt_t pmt_make_dict();

//! Return a new dictionary with \p key associated with \p value.
pmt_t pmt_dict_add(const pmt_t &dict, const pmt_t &key, const pmt_t &value);

//! Return a new dictionary with \p key removed.
pmt_t pmt_dict_delete(const pmt_t &dict, const pmt_t &key);

//! Return true if \p key exists in \p dict
bool  pmt_dict_has_key(const pmt_t &dict, const pmt_t &key);

//! If \p key exists in \p dict, return associated value; otherwise return \p not_found.
pmt_t pmt_dict_ref(const pmt_t &dict, const pmt_t &key, const pmt_t &not_found);

//! Return list of (key . value) pairs
pmt_t pmt_dict_items(pmt_t dict);

//! Return list of keys
pmt_t pmt_dict_keys(pmt_t dict);

//! Return list of values
pmt_t pmt_dict_values(pmt_t dict);

/*
 * ------------------------------------------------------------------------
 *   Any (wraps boost::any -- can be used to wrap pretty much anything)
 *
 * Cannot be serialized or used across process boundaries.
 * See http://www.boost.org/doc/html/any.html
 * ------------------------------------------------------------------------
 */

//! Return true if \p obj is an any
bool pmt_is_any(pmt_t obj);

//! make an any
pmt_t pmt_make_any(const boost::any &any);

//! Return underlying boost::any
boost::any pmt_any_ref(pmt_t obj);

//! Store \p any in \p obj
void pmt_any_set(pmt_t obj, const boost::any &any);


/*
 * ------------------------------------------------------------------------
 *    msg_accepter -- pmt representation of gruel::msg_accepter
 * ------------------------------------------------------------------------
 */
//! Return true if \p obj is a msg_accepter
bool pmt_is_msg_accepter(const pmt_t &obj);

//! make a msg_accepter
pmt_t pmt_make_msg_accepter(boost::shared_ptr<gruel::msg_accepter> ma);

//! Return underlying msg_accepter
boost::shared_ptr<gruel::msg_accepter> pmt_msg_accepter_ref(const pmt_t &obj);

/*
 * ------------------------------------------------------------------------
 *			  General functions
 * ------------------------------------------------------------------------
 */

//! Return true if x and y are the same object; otherwise return false.
bool pmt_eq(const pmt_t& x, const pmt_t& y);

/*!
 * \brief Return true if x and y should normally be regarded as the same object, else false.
 *
 * <pre>
 * eqv returns true if:
 *   x and y are the same object.
 *   x and y are both \#t or both \#f.
 *   x and y are both symbols and their names are the same.
 *   x and y are both numbers, and are numerically equal.
 *   x and y are both the empty list (nil).
 *   x and y are pairs or vectors that denote same location in store.
 * </pre>
 */
bool pmt_eqv(const pmt_t& x, const pmt_t& y);

/*!
 * pmt_equal recursively compares the contents of pairs and vectors,
 * applying pmt_eqv on other objects such as numbers and symbols.  
 * pmt_equal may fail to terminate if its arguments are circular data
 * structures.
 */
bool pmt_equal(const pmt_t& x, const pmt_t& y);


//! Return the number of elements in v
size_t pmt_length(const pmt_t& v);

/*!
 * \brief Find the first pair in \p alist whose car field is \p obj
 *  and return that pair.
 *
 * \p alist (for "association list") must be a list of pairs.  If no pair
 * in \p alist has \p obj as its car then \#f is returned.
 * Uses pmt_eq to compare \p obj with car fields of the pairs in \p alist.
 */
pmt_t pmt_assq(pmt_t obj, pmt_t alist);

/*!
 * \brief Find the first pair in \p alist whose car field is \p obj
 *  and return that pair.
 *
 * \p alist (for "association list") must be a list of pairs.  If no pair
 * in \p alist has \p obj as its car then \#f is returned.
 * Uses pmt_eqv to compare \p obj with car fields of the pairs in \p alist.
 */
pmt_t pmt_assv(pmt_t obj, pmt_t alist);

/*!
 * \brief Find the first pair in \p alist whose car field is \p obj
 *  and return that pair.
 *
 * \p alist (for "association list") must be a list of pairs.  If no pair
 * in \p alist has \p obj as its car then \#f is returned.
 * Uses pmt_equal to compare \p obj with car fields of the pairs in \p alist.
 */
pmt_t pmt_assoc(pmt_t obj, pmt_t alist);

/*!
 * \brief Apply \p proc element-wise to the elements of list and returns
 * a list of the results, in order.
 *
 * \p list must be a list.  The dynamic order in which \p proc is
 * applied to the elements of \p list is unspecified.
 */
pmt_t pmt_map(pmt_t proc(const pmt_t&), pmt_t list);

/*!
 * \brief reverse \p list.
 *
 * \p list must be a proper list.
 */
pmt_t pmt_reverse(pmt_t list);

/*!
 * \brief destructively reverse \p list.
 *
 * \p list must be a proper list.
 */
pmt_t pmt_reverse_x(pmt_t list);

/*!
 * \brief (acons x y a) == (cons (cons x y) a)
 */
inline static pmt_t
pmt_acons(pmt_t x, pmt_t y, pmt_t a)
{
  return pmt_cons(pmt_cons(x, y), a);
}

/*!
 * \brief locates \p nth element of \n list where the car is the 'zeroth' element.
 */
pmt_t pmt_nth(size_t n, pmt_t list);

/*!
 * \brief returns the tail of \p list that would be obtained by calling
 * cdr \p n times in succession.
 */
pmt_t pmt_nthcdr(size_t n, pmt_t list);

/*!
 * \brief Return the first sublist of \p list whose car is \p obj.
 * If \p obj does not occur in \p list, then \#f is returned.
 * pmt_memq use pmt_eq to compare \p obj with the elements of \p list.
 */
pmt_t pmt_memq(pmt_t obj, pmt_t list);

/*!
 * \brief Return the first sublist of \p list whose car is \p obj.
 * If \p obj does not occur in \p list, then \#f is returned.
 * pmt_memv use pmt_eqv to compare \p obj with the elements of \p list.
 */
pmt_t pmt_memv(pmt_t obj, pmt_t list);

/*!
 * \brief Return the first sublist of \p list whose car is \p obj.
 * If \p obj does not occur in \p list, then \#f is returned.
 * pmt_member use pmt_equal to compare \p obj with the elements of \p list.
 */
pmt_t pmt_member(pmt_t obj, pmt_t list);

/*!
 * \brief Return true if every element of \p list1 appears in \p list2, and false otherwise.
 * Comparisons are done with pmt_eqv.
 */
bool pmt_subsetp(pmt_t list1, pmt_t list2);

/*!
 * \brief Return a list of length 1 containing \p x1
 */
pmt_t pmt_list1(const pmt_t& x1);

/*!
 * \brief Return a list of length 2 containing \p x1, \p x2
 */
pmt_t pmt_list2(const pmt_t& x1, const pmt_t& x2);

/*!
 * \brief Return a list of length 3 containing \p x1, \p x2, \p x3
 */
pmt_t pmt_list3(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3);

/*!
 * \brief Return a list of length 4 containing \p x1, \p x2, \p x3, \p x4
 */
pmt_t pmt_list4(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4);

/*!
 * \brief Return a list of length 5 containing \p x1, \p x2, \p x3, \p x4, \p x5
 */
pmt_t pmt_list5(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4, const pmt_t& x5);

/*!
 * \brief Return a list of length 6 containing \p x1, \p x2, \p x3, \p x4, \p
 * x5, \p x6
 */
pmt_t pmt_list6(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4, const pmt_t& x5, const pmt_t& x6);

/*!
 * \brief Return \p list with \p item added to it.
 */
pmt_t pmt_list_add(pmt_t list, const pmt_t& item);


/*
 * ------------------------------------------------------------------------
 *			     read / write
 * ------------------------------------------------------------------------
 */
extern const pmt_t PMT_EOF;	//< The end of file object

//! return true if obj is the EOF object, otherwise return false.
bool pmt_is_eof_object(pmt_t obj);

/*!
 * read converts external representations of pmt objects into the
 * objects themselves.  Read returns the next object parsable from
 * the given input port, updating port to point to the first
 * character past the end of the external representation of the
 * object.
 *
 * If an end of file is encountered in the input before any
 * characters are found that can begin an object, then an end of file
 * object is returned.   The port remains open, and further attempts
 * to read will also return an end of file object.  If an end of file
 * is encountered after the beginning of an object's external
 * representation, but the external representation is incomplete and
 * therefore not parsable, an error is signaled.
 */
pmt_t pmt_read(std::istream &port);

/*!
 * Write a written representation of \p obj to the given \p port.
 */
void pmt_write(pmt_t obj, std::ostream &port);

/*!
 * Return a string representation of \p obj.
 * This is the same output as would be generated by pmt_write.
 */
std::string pmt_write_string(pmt_t obj);


/*!
 * \brief Write pmt string representation to stdout.
 */
void pmt_print(pmt_t v);


/*
 * ------------------------------------------------------------------------
 *		      portable byte stream representation
 * ------------------------------------------------------------------------
 */
/*!
 * \brief Write portable byte-serial representation of \p obj to \p sink
 */
bool pmt_serialize(pmt_t obj, std::streambuf &sink);

/*!
 * \brief Create obj from portable byte-serial representation
 */
pmt_t pmt_deserialize(std::streambuf &source);


void pmt_dump_sizeof();	// debugging

/*!
 * \brief Provide a simple string generating interface to pmt's serialize function
 */
std::string pmt_serialize_str(pmt_t obj);

/*!
 * \brief Provide a simple string generating interface to pmt's deserialize function
 */
pmt_t pmt_deserialize_str(std::string str);

} //namespace pmt

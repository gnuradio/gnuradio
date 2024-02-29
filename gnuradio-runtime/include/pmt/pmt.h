/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PMT_H
#define INCLUDED_PMT_H

#include <pmt/api.h>

#include <string_view>
#include <any>
#include <complex>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace gr {
namespace messages {
class msg_accepter;
}
} // namespace gr

/*!
 * This file defines a polymorphic type and the operations on it.
 *
 * It draws heavily on the idea of scheme and lisp data types.
 * The interface parallels that in Guile 1.8, with the notable
 * exception that these objects are transparently reference counted.
 */

namespace pmt {

/*!
 * \brief base class of all pmt types
 */
class PMT_API pmt_base
{

public:
    pmt_base(){};
    pmt_base(const pmt_base&) = delete;
    virtual ~pmt_base();

    virtual bool is_bool() const { return false; }
    virtual bool is_symbol() const { return false; }
    virtual bool is_number() const { return false; }
    virtual bool is_integer() const { return false; }
    virtual bool is_uint64() const { return false; }
    virtual bool is_real() const { return false; }
    virtual bool is_complex() const { return false; }
    virtual bool is_null() const { return false; }
    virtual bool is_pair() const { return false; }
    virtual bool is_tuple() const { return false; }
    virtual bool is_vector() const { return false; }
    virtual bool is_dict() const { return false; }
    virtual bool is_any() const { return false; }

    virtual bool is_uniform_vector() const { return false; }
    virtual bool is_u8vector() const { return false; }
    virtual bool is_s8vector() const { return false; }
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

/*!
 * \brief typedef for shared pointer (transparent reference counting).
 */
typedef std::shared_ptr<pmt_base> pmt_t;

class PMT_API exception : public std::logic_error
{
public:
    exception(const std::string& msg, pmt_t obj);
};

class PMT_API wrong_type : public std::invalid_argument
{
public:
    wrong_type(const std::string& msg, pmt_t obj);
};

class PMT_API out_of_range : public exception
{
public:
    out_of_range(const std::string& msg, pmt_t obj);
};

class PMT_API notimplemented : public exception
{
public:
    notimplemented(const std::string& msg, pmt_t obj);
};


/*
 * ------------------------------------------------------------------------
 * Constants
 * ------------------------------------------------------------------------
 */

PMT_API pmt_t get_PMT_NIL();
PMT_API pmt_t get_PMT_T();
PMT_API pmt_t get_PMT_F();
PMT_API pmt_t get_PMT_EOF();

#define PMT_NIL get_PMT_NIL()
#define PMT_T get_PMT_T()
#define PMT_F get_PMT_F()
#define PMT_EOF get_PMT_EOF()


/*
 * ------------------------------------------------------------------------
 * Booleans.  Two constants, #t and #f.
 *
 * In predicates, anything that is not #f is considered true.
 * I.e., there is a single false value, #f.
 * ------------------------------------------------------------------------
 */

//! Return true if obj is \#t or \#f, else return false.
PMT_API bool is_bool(pmt_t obj);

//! Return false if obj is \#f, else return true.
PMT_API bool is_true(pmt_t obj);

//! Return true if obj is \#f, else return true.
PMT_API bool is_false(pmt_t obj);

//! Return \#f is val is false, else return \#t.
PMT_API pmt_t from_bool(bool val);

//! Return true if val is pmt::True, return false when val is pmt::PMT_F,
// else raise wrong_type exception.
PMT_API bool to_bool(pmt_t val);

/*
 * ------------------------------------------------------------------------
 *			       Symbols
 * ------------------------------------------------------------------------
 */

//! Return true if obj is a symbol, else false.
PMT_API bool is_symbol(const pmt_t& obj);

//! Return the symbol whose name is \p s.
PMT_API pmt_t string_to_symbol(std::string_view s);

//! Alias for pmt_string_to_symbol
PMT_API pmt_t intern(std::string_view s);


/*!
 * If \p is a symbol, return the name of the symbol as a string.
 * Otherwise, raise the wrong_type exception.
 */
PMT_API const std::string symbol_to_string(const pmt_t& sym);

/*
 * ------------------------------------------------------------------------
 *           Numbers: we support integer, real and complex
 * ------------------------------------------------------------------------
 */

//! Return true if obj is any kind of number, else false.
PMT_API bool is_number(pmt_t obj);

/*
 * ------------------------------------------------------------------------
 *			       Integers
 * ------------------------------------------------------------------------
 */

//! Return true if \p x is an integer number, else false
PMT_API bool is_integer(pmt_t x);

//! Return the pmt value that represents the integer \p x.
PMT_API pmt_t from_long(long x);

/*!
 * \brief Convert pmt to long if possible.
 *
 * When \p x represents an exact integer that fits in a long,
 * return that integer.  Else raise an exception, either wrong_type
 * when x is not an exact integer, or out_of_range when it doesn't fit.
 */
PMT_API long to_long(pmt_t x);

/*
 * ------------------------------------------------------------------------
 *			       uint64_t
 * ------------------------------------------------------------------------
 */

//! Return true if \p x is an uint64 number, else false
PMT_API bool is_uint64(pmt_t x);

//! Return the pmt value that represents the uint64 \p x.
PMT_API pmt_t from_uint64(uint64_t x);

/*!
 * \brief Convert pmt to uint64 if possible.
 *
 * When \p x represents an exact integer that fits in a uint64,
 * return that uint64.  Else raise an exception, either wrong_type
 * when x is not an exact uint64, or out_of_range when it doesn't fit.
 */
PMT_API uint64_t to_uint64(pmt_t x);

/*
 * ------------------------------------------------------------------------
 *				Reals
 * ------------------------------------------------------------------------
 */

/*
 * \brief Return true if \p obj is a real number, else false.
 */
PMT_API bool is_real(pmt_t obj);

//! Return the pmt value that represents double \p x.
PMT_API pmt_t from_double(double x);
PMT_API pmt_t from_float(float x);

/*!
 * \brief Convert pmt to double if possible.
 *
 * Returns the number closest to \p val that is representable
 * as a double.  The argument \p val must be a real or integer, otherwise
 * a wrong_type exception is raised.
 */
PMT_API double to_double(pmt_t x);

/*!
 * \brief Convert pmt to float if possible.
 *
 * This basically is to_double() with a type-cast; the PMT stores
 * the value as a double in any case. Use this when strict typing
 * is required.
 */
PMT_API float to_float(pmt_t x);

/*
 * ------------------------------------------------------------------------
 *			       Complex
 * ------------------------------------------------------------------------
 */

/*!
 * \brief return true if \p obj is a complex number, false otherwise.
 */
PMT_API bool is_complex(pmt_t obj);

//! Return a complex number constructed of the given real and imaginary parts.
PMT_API pmt_t make_rectangular(double re, double im);

//! Return a complex number constructed of the given real and imaginary parts.
PMT_API pmt_t from_complex(double re, double im);

//! Return a complex number constructed of the given a complex number.
PMT_API pmt_t from_complex(const std::complex<double>& z);

//! Return a complex number constructed of the given real and imaginary parts.
PMT_API pmt_t pmt_from_complex(double re, double im);

//! Return a complex number constructed of the given a complex number.
PMT_API pmt_t pmt_from_complex(const std::complex<double>& z);

/*!
 * If \p z is complex, real or integer, return the closest complex<double>.
 * Otherwise, raise the wrong_type exception.
 */
PMT_API std::complex<double> to_complex(pmt_t z);

/*
 * ------------------------------------------------------------------------
 *				Pairs
 * ------------------------------------------------------------------------
 */

//! Return true if \p x is the empty list, otherwise return false.
PMT_API bool is_null(const pmt_t& x);

//! Return true if \p obj is a pair, else false (warning: also returns true for a dict)
PMT_API bool is_pair(const pmt_t& obj);

//! Return a newly allocated pair whose car is \p x and whose cdr is \p y.
PMT_API pmt_t cons(const pmt_t& x, const pmt_t& y);

//! If \p pair is a pair, return the car of the \p pair, otherwise raise wrong_type.
PMT_API pmt_t car(const pmt_t& pair);

//! If \p pair is a pair, return the cdr of the \p pair, otherwise raise wrong_type.
PMT_API pmt_t cdr(const pmt_t& pair);

//! Stores \p value in the car field of \p pair.
PMT_API void set_car(pmt_t pair, pmt_t value);

//! Stores \p value in the cdr field of \p pair.
PMT_API void set_cdr(pmt_t pair, pmt_t value);

PMT_API pmt_t caar(pmt_t pair);
PMT_API pmt_t cadr(pmt_t pair);
PMT_API pmt_t cdar(pmt_t pair);
PMT_API pmt_t cddr(pmt_t pair);
PMT_API pmt_t caddr(pmt_t pair);
PMT_API pmt_t cadddr(pmt_t pair);

/*
 * ------------------------------------------------------------------------
 *			          Tuples
 *
 * Store a fixed number of objects.  Tuples are not modifiable, and thus
 * are excellent for use as messages.  Indexing is zero based.
 * Access time to an element is O(1).
 * ------------------------------------------------------------------------
 */

//! Return true if \p x is a tuple, otherwise false.
PMT_API bool is_tuple(pmt_t x);

PMT_API pmt_t make_tuple();
PMT_API pmt_t make_tuple(const pmt_t& e0);
PMT_API pmt_t make_tuple(const pmt_t& e0, const pmt_t& e1);
PMT_API pmt_t make_tuple(const pmt_t& e0, const pmt_t& e1, const pmt_t& e2);
PMT_API pmt_t make_tuple(const pmt_t& e0,
                         const pmt_t& e1,
                         const pmt_t& e2,
                         const pmt_t& e3);
PMT_API pmt_t make_tuple(
    const pmt_t& e0, const pmt_t& e1, const pmt_t& e2, const pmt_t& e3, const pmt_t& e4);
PMT_API pmt_t make_tuple(const pmt_t& e0,
                         const pmt_t& e1,
                         const pmt_t& e2,
                         const pmt_t& e3,
                         const pmt_t& e4,
                         const pmt_t& e5);
PMT_API pmt_t make_tuple(const pmt_t& e0,
                         const pmt_t& e1,
                         const pmt_t& e2,
                         const pmt_t& e3,
                         const pmt_t& e4,
                         const pmt_t& e5,
                         const pmt_t& e6);
PMT_API pmt_t make_tuple(const pmt_t& e0,
                         const pmt_t& e1,
                         const pmt_t& e2,
                         const pmt_t& e3,
                         const pmt_t& e4,
                         const pmt_t& e5,
                         const pmt_t& e6,
                         const pmt_t& e7);
PMT_API pmt_t make_tuple(const pmt_t& e0,
                         const pmt_t& e1,
                         const pmt_t& e2,
                         const pmt_t& e3,
                         const pmt_t& e4,
                         const pmt_t& e5,
                         const pmt_t& e6,
                         const pmt_t& e7,
                         const pmt_t& e8);
PMT_API pmt_t make_tuple(const pmt_t& e0,
                         const pmt_t& e1,
                         const pmt_t& e2,
                         const pmt_t& e3,
                         const pmt_t& e4,
                         const pmt_t& e5,
                         const pmt_t& e6,
                         const pmt_t& e7,
                         const pmt_t& e8,
                         const pmt_t& e9);

/*!
 * If \p x is a vector or proper list, return a tuple containing the elements of x
 */
PMT_API pmt_t to_tuple(const pmt_t& x);

/*!
 * Return the contents of position \p k of \p tuple.
 * \p k must be a valid index of \p tuple.
 */
PMT_API pmt_t tuple_ref(const pmt_t& tuple, size_t k);

/*
 * ------------------------------------------------------------------------
 *			       Vectors
 *
 * These vectors can hold any kind of objects.  Indexing is zero based.
 * ------------------------------------------------------------------------
 */

//! Return true if \p x is a vector, otherwise false.
PMT_API bool is_vector(pmt_t x);

//! Make a vector of length \p k, with initial values set to \p fill
PMT_API pmt_t make_vector(size_t k, pmt_t fill);

/*!
 * Return the contents of position \p k of \p vector.
 * \p k must be a valid index of \p vector.
 */
PMT_API pmt_t vector_ref(pmt_t vector, size_t k);

//! Store \p obj in position \p k.
PMT_API void vector_set(pmt_t vector, size_t k, pmt_t obj);

//! Store \p fill in every position of \p vector
PMT_API void vector_fill(pmt_t vector, pmt_t fill);

/*
 * ------------------------------------------------------------------------
 *		      Binary Large Objects (BLOBs)
 *
 * Handy for passing around uninterpreted chunks of memory.
 * ------------------------------------------------------------------------
 */

//! Return true if \p x is a blob, otherwise false.
PMT_API bool is_blob(pmt_t x);

/*!
 * \brief Make a blob given a pointer and length in bytes
 *
 * \param buf is the pointer to data to use to create blob
 * \param len is the size of the data in bytes.
 *
 * The data is copied into the blob.
 */
PMT_API pmt_t make_blob(const void* buf, size_t len);

//! Return a pointer to the blob's data
PMT_API const void* blob_data(pmt_t blob);

//! Return the blob's length in bytes
PMT_API size_t blob_length(pmt_t blob);

/*!
 * <pre>
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
 * </pre>
 */

//! true if \p x is any kind of uniform numeric vector
PMT_API bool is_uniform_vector(pmt_t x);

PMT_API bool is_u8vector(pmt_t x);
PMT_API bool is_s8vector(pmt_t x);
PMT_API bool is_u16vector(pmt_t x);
PMT_API bool is_s16vector(pmt_t x);
PMT_API bool is_u32vector(pmt_t x);
PMT_API bool is_s32vector(pmt_t x);
PMT_API bool is_u64vector(pmt_t x);
PMT_API bool is_s64vector(pmt_t x);
PMT_API bool is_f32vector(pmt_t x);
PMT_API bool is_f64vector(pmt_t x);
PMT_API bool is_c32vector(pmt_t x);
PMT_API bool is_c64vector(pmt_t x);

//! item size in bytes if \p x is any kind of uniform numeric vector
PMT_API size_t uniform_vector_itemsize(pmt_t x);

PMT_API pmt_t make_u8vector(size_t k, uint8_t fill);
PMT_API pmt_t make_s8vector(size_t k, int8_t fill);
PMT_API pmt_t make_u16vector(size_t k, uint16_t fill);
PMT_API pmt_t make_s16vector(size_t k, int16_t fill);
PMT_API pmt_t make_u32vector(size_t k, uint32_t fill);
PMT_API pmt_t make_s32vector(size_t k, int32_t fill);
PMT_API pmt_t make_u64vector(size_t k, uint64_t fill);
PMT_API pmt_t make_s64vector(size_t k, int64_t fill);
PMT_API pmt_t make_f32vector(size_t k, float fill);
PMT_API pmt_t make_f64vector(size_t k, double fill);
PMT_API pmt_t make_c32vector(size_t k, std::complex<float> fill);
PMT_API pmt_t make_c64vector(size_t k, std::complex<double> fill);

PMT_API pmt_t init_u8vector(size_t k, const uint8_t* data);
PMT_API pmt_t init_u8vector(size_t k, const std::vector<uint8_t>& data);
PMT_API pmt_t init_s8vector(size_t k, const int8_t* data);
PMT_API pmt_t init_s8vector(size_t k, const std::vector<int8_t>& data);
PMT_API pmt_t init_u16vector(size_t k, const uint16_t* data);
PMT_API pmt_t init_u16vector(size_t k, const std::vector<uint16_t>& data);
PMT_API pmt_t init_s16vector(size_t k, const int16_t* data);
PMT_API pmt_t init_s16vector(size_t k, const std::vector<int16_t>& data);
PMT_API pmt_t init_u32vector(size_t k, const uint32_t* data);
PMT_API pmt_t init_u32vector(size_t k, const std::vector<uint32_t>& data);
PMT_API pmt_t init_s32vector(size_t k, const int32_t* data);
PMT_API pmt_t init_s32vector(size_t k, const std::vector<int32_t>& data);
PMT_API pmt_t init_u64vector(size_t k, const uint64_t* data);
PMT_API pmt_t init_u64vector(size_t k, const std::vector<uint64_t>& data);
PMT_API pmt_t init_s64vector(size_t k, const int64_t* data);
PMT_API pmt_t init_s64vector(size_t k, const std::vector<int64_t>& data);
PMT_API pmt_t init_f32vector(size_t k, const float* data);
PMT_API pmt_t init_f32vector(size_t k, const std::vector<float>& data);
PMT_API pmt_t init_f64vector(size_t k, const double* data);
PMT_API pmt_t init_f64vector(size_t k, const std::vector<double>& data);
PMT_API pmt_t init_c32vector(size_t k, const std::complex<float>* data);
PMT_API pmt_t init_c32vector(size_t k, const std::vector<std::complex<float>>& data);
PMT_API pmt_t init_c64vector(size_t k, const std::complex<double>* data);
PMT_API pmt_t init_c64vector(size_t k, const std::vector<std::complex<double>>& data);

PMT_API uint8_t u8vector_ref(pmt_t v, size_t k);
PMT_API int8_t s8vector_ref(pmt_t v, size_t k);
PMT_API uint16_t u16vector_ref(pmt_t v, size_t k);
PMT_API int16_t s16vector_ref(pmt_t v, size_t k);
PMT_API uint32_t u32vector_ref(pmt_t v, size_t k);
PMT_API int32_t s32vector_ref(pmt_t v, size_t k);
PMT_API uint64_t u64vector_ref(pmt_t v, size_t k);
PMT_API int64_t s64vector_ref(pmt_t v, size_t k);
PMT_API float f32vector_ref(pmt_t v, size_t k);
PMT_API double f64vector_ref(pmt_t v, size_t k);
PMT_API std::complex<float> c32vector_ref(pmt_t v, size_t k);
PMT_API std::complex<double> c64vector_ref(pmt_t v, size_t k);

PMT_API void u8vector_set(pmt_t v, size_t k, uint8_t x); //< v[k] = x
PMT_API void s8vector_set(pmt_t v, size_t k, int8_t x);
PMT_API void u16vector_set(pmt_t v, size_t k, uint16_t x);
PMT_API void s16vector_set(pmt_t v, size_t k, int16_t x);
PMT_API void u32vector_set(pmt_t v, size_t k, uint32_t x);
PMT_API void s32vector_set(pmt_t v, size_t k, int32_t x);
PMT_API void u64vector_set(pmt_t v, size_t k, uint64_t x);
PMT_API void s64vector_set(pmt_t v, size_t k, int64_t x);
PMT_API void f32vector_set(pmt_t v, size_t k, float x);
PMT_API void f64vector_set(pmt_t v, size_t k, double x);
PMT_API void c32vector_set(pmt_t v, size_t k, std::complex<float> x);
PMT_API void c64vector_set(pmt_t v, size_t k, std::complex<double> x);

// Return const pointers to the elements

PMT_API const void*
uniform_vector_elements(pmt_t v, size_t& len); //< works with any; len is in bytes

PMT_API const uint8_t* u8vector_elements(pmt_t v, size_t& len);   //< len is in elements
PMT_API const int8_t* s8vector_elements(pmt_t v, size_t& len);    //< len is in elements
PMT_API const uint16_t* u16vector_elements(pmt_t v, size_t& len); //< len is in elements
PMT_API const int16_t* s16vector_elements(pmt_t v, size_t& len);  //< len is in elements
PMT_API const uint32_t* u32vector_elements(pmt_t v, size_t& len); //< len is in elements
PMT_API const int32_t* s32vector_elements(pmt_t v, size_t& len);  //< len is in elements
PMT_API const uint64_t* u64vector_elements(pmt_t v, size_t& len); //< len is in elements
PMT_API const int64_t* s64vector_elements(pmt_t v, size_t& len);  //< len is in elements
PMT_API const float* f32vector_elements(pmt_t v, size_t& len);    //< len is in elements
PMT_API const double* f64vector_elements(pmt_t v, size_t& len);   //< len is in elements
PMT_API const std::complex<float>* c32vector_elements(pmt_t v,
                                                      size_t& len); //< len is in elements
PMT_API const std::complex<double>*
c64vector_elements(pmt_t v, size_t& len); //< len is in elements

// len is in elements
PMT_API const std::vector<uint8_t> u8vector_elements(pmt_t v);
PMT_API const std::vector<int8_t> s8vector_elements(pmt_t v);
PMT_API const std::vector<uint16_t> u16vector_elements(pmt_t v);
PMT_API const std::vector<int16_t> s16vector_elements(pmt_t v);
PMT_API const std::vector<uint32_t> u32vector_elements(pmt_t v);
PMT_API const std::vector<int32_t> s32vector_elements(pmt_t v);
PMT_API const std::vector<uint64_t> u64vector_elements(pmt_t v);
PMT_API const std::vector<int64_t> s64vector_elements(pmt_t v);
PMT_API const std::vector<float> f32vector_elements(pmt_t v);
PMT_API const std::vector<double> f64vector_elements(pmt_t v);
PMT_API const std::vector<std::complex<float>> c32vector_elements(pmt_t v);
PMT_API const std::vector<std::complex<double>> c64vector_elements(pmt_t v);

// Return non-const pointers to the elements

PMT_API void*
uniform_vector_writable_elements(pmt_t v,
                                 size_t& len); //< works with any; len is in bytes

PMT_API uint8_t* u8vector_writable_elements(pmt_t v, size_t& len); //< len is in elements
PMT_API int8_t* s8vector_writable_elements(pmt_t v, size_t& len);  //< len is in elements
PMT_API uint16_t* u16vector_writable_elements(pmt_t v,
                                              size_t& len);         //< len is in elements
PMT_API int16_t* s16vector_writable_elements(pmt_t v, size_t& len); //< len is in elements
PMT_API uint32_t* u32vector_writable_elements(pmt_t v,
                                              size_t& len);         //< len is in elements
PMT_API int32_t* s32vector_writable_elements(pmt_t v, size_t& len); //< len is in elements
PMT_API uint64_t* u64vector_writable_elements(pmt_t v,
                                              size_t& len);         //< len is in elements
PMT_API int64_t* s64vector_writable_elements(pmt_t v, size_t& len); //< len is in elements
PMT_API float* f32vector_writable_elements(pmt_t v, size_t& len);   //< len is in elements
PMT_API double* f64vector_writable_elements(pmt_t v, size_t& len);  //< len is in elements
PMT_API std::complex<float>*
c32vector_writable_elements(pmt_t v, size_t& len); //< len is in elements
PMT_API std::complex<double>*
c64vector_writable_elements(pmt_t v, size_t& len); //< len is in elements

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
PMT_API bool is_dict(const pmt_t& obj);

//! Return a newly allocated dict whose car is a key-value pair \p x and whose cdr is a
//! dict \p y.
PMT_API pmt_t dcons(const pmt_t& x, const pmt_t& y);

//! Make an empty dictionary
PMT_API pmt_t make_dict();

/*!
 * \brief Make a dictionary from an existing mapping type.
 * The constraint for this to work is the ability for the map_t to iterate over [key,
 * value] pairs, that is, to be able to be used in a
 *
 * <pre>for(const auto& [key, val] : prototype)</pre>
 *
 * loop.
 */
template <typename map_t>
pmt_t dict_from_mapping(const map_t& prototype)
{
    pmt_t protodict = make_dict();
    for (const auto& [key, value] : prototype) {
        protodict = dict_add(protodict, key, value);
    }
    return protodict;
}

//! Return a new dictionary with \p key associated with \p value.
PMT_API pmt_t dict_add(const pmt_t& dict, const pmt_t& key, const pmt_t& value);

//! Return a new dictionary with \p key removed.
PMT_API pmt_t dict_delete(const pmt_t& dict, const pmt_t& key);

//! Return true if \p key exists in \p dict
PMT_API bool dict_has_key(const pmt_t& dict, const pmt_t& key);

//! If \p key exists in \p dict, return associated value; otherwise return \p not_found.
PMT_API pmt_t dict_ref(const pmt_t& dict, const pmt_t& key, const pmt_t& not_found);

//! Return list of (key . value) pairs
PMT_API pmt_t dict_items(pmt_t dict);

//! Return list of keys
PMT_API pmt_t dict_keys(pmt_t dict);

//! Return a new dictionary \p dict1 with k=>v pairs from \p dict2 added.
PMT_API pmt_t dict_update(const pmt_t& dict1, const pmt_t& dict2);

//! Return list of values
PMT_API pmt_t dict_values(pmt_t dict);

/*
 * ------------------------------------------------------------------------
 *   Any (wraps std::any -- can be used to wrap pretty much anything)
 *
 * Cannot be serialized or used across process boundaries.
 * See http://www.boost.org/doc/html/any.html
 * ------------------------------------------------------------------------
 */

//! Return true if \p obj is an any
PMT_API bool is_any(pmt_t obj);

//! make an any
PMT_API pmt_t make_any(const std::any& any);

//! Return underlying std::any
PMT_API std::any any_ref(pmt_t obj);

//! Store \p any in \p obj
PMT_API void any_set(pmt_t obj, const std::any& any);


/*
 * ------------------------------------------------------------------------
 *    msg_accepter -- pmt representation of pmt::msg_accepter
 * ------------------------------------------------------------------------
 */
//! Return true if \p obj is a msg_accepter
PMT_API bool is_msg_accepter(const pmt_t& obj);

//! make a msg_accepter
PMT_API pmt_t make_msg_accepter(std::shared_ptr<gr::messages::msg_accepter> ma);

//! Return underlying msg_accepter
PMT_API std::shared_ptr<gr::messages::msg_accepter> msg_accepter_ref(const pmt_t& obj);

/*
 * ------------------------------------------------------------------------
 *			  General functions
 * ------------------------------------------------------------------------
 */

/*!
 * Returns true if the object is a PDU meaning:
 *   the object is a pair
 *   the car is a dictionary type object (including an empty dict)
 *   the cdr is a uniform vector of any type
 */
PMT_API bool is_pdu(const pmt_t& obj);

//! Return true if x and y are the same object; otherwise return false.
PMT_API bool eq(const pmt_t& x, const pmt_t& y);

/*!
 * \brief Return true if x and y should normally be regarded as the same object, else
 * false.
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
PMT_API bool eqv(const pmt_t& x, const pmt_t& y);

/*!
 * pmt::equal recursively compares the contents of pairs and vectors,
 * applying pmt::eqv on other objects such as numbers and symbols.
 * pmt::equal may fail to terminate if its arguments are circular data
 * structures.
 */
PMT_API bool equal(const pmt_t& x, const pmt_t& y);


//! Return the number of elements in v
PMT_API size_t length(const pmt_t& v);

/*!
 * \brief Find the first pair in \p alist whose car field is \p obj
 *  and return that pair.
 *
 * \p alist (for "association list") must be a list of pairs.  If no pair
 * in \p alist has \p obj as its car then \#f is returned.
 * Uses pmt::eq to compare \p obj with car fields of the pairs in \p alist.
 */
PMT_API pmt_t assq(pmt_t obj, pmt_t alist);

/*!
 * \brief Find the first pair in \p alist whose car field is \p obj
 *  and return that pair.
 *
 * \p alist (for "association list") must be a list of pairs.  If no pair
 * in \p alist has \p obj as its car then \#f is returned.
 * Uses pmt::eqv to compare \p obj with car fields of the pairs in \p alist.
 */
PMT_API pmt_t assv(pmt_t obj, pmt_t alist);

/*!
 * \brief Find the first pair in \p alist whose car field is \p obj
 *  and return that pair.
 *
 * \p alist (for "association list") must be a list of pairs.  If no pair
 * in \p alist has \p obj as its car then \#f is returned.
 * Uses pmt::equal to compare \p obj with car fields of the pairs in \p alist.
 */
PMT_API pmt_t assoc(pmt_t obj, pmt_t alist);

/*!
 * \brief Apply \p proc element-wise to the elements of list and returns
 * a list of the results, in order.
 *
 * \p list must be a list.  The dynamic order in which \p proc is
 * applied to the elements of \p list is unspecified.
 */
PMT_API pmt_t map(pmt_t proc(const pmt_t&), pmt_t list);

/*!
 * \brief reverse \p list.
 *
 * \p list must be a proper list.
 */
PMT_API pmt_t reverse(pmt_t list);

/*!
 * \brief destructively reverse \p list.
 *
 * \p list must be a proper list.
 */
PMT_API pmt_t reverse_x(pmt_t list);

/*!
 * \brief (acons x y a) == (cons (cons x y) a)
 */
inline static pmt_t acons(pmt_t x, pmt_t y, pmt_t a) { return dcons(cons(x, y), a); }

/*!
 * \brief locates \p nth element of \n list where the car is the 'zeroth' element.
 */
PMT_API pmt_t nth(size_t n, pmt_t list);

/*!
 * \brief returns the tail of \p list that would be obtained by calling
 * cdr \p n times in succession.
 */
PMT_API pmt_t nthcdr(size_t n, pmt_t list);

/*!
 * \brief Return the first sublist of \p list whose car is \p obj.
 * If \p obj does not occur in \p list, then \#f is returned.
 * pmt::memq use pmt::eq to compare \p obj with the elements of \p list.
 */
PMT_API pmt_t memq(pmt_t obj, pmt_t list);

/*!
 * \brief Return the first sublist of \p list whose car is \p obj.
 * If \p obj does not occur in \p list, then \#f is returned.
 * pmt::memv use pmt::eqv to compare \p obj with the elements of \p list.
 */
PMT_API pmt_t memv(pmt_t obj, pmt_t list);

/*!
 * \brief Return the first sublist of \p list whose car is \p obj.
 * If \p obj does not occur in \p list, then \#f is returned.
 * pmt::member use pmt::equal to compare \p obj with the elements of \p list.
 */
PMT_API pmt_t member(pmt_t obj, pmt_t list);

/*!
 * \brief Return true if every element of \p list1 appears in \p list2, and false
 * otherwise. Comparisons are done with pmt::eqv.
 */
PMT_API bool subsetp(pmt_t list1, pmt_t list2);

/*!
 * \brief Return a list of length 1 containing \p x1
 */
PMT_API pmt_t list1(const pmt_t& x1);

/*!
 * \brief Return a list of length 2 containing \p x1, \p x2
 */
PMT_API pmt_t list2(const pmt_t& x1, const pmt_t& x2);

/*!
 * \brief Return a list of length 3 containing \p x1, \p x2, \p x3
 */
PMT_API pmt_t list3(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3);

/*!
 * \brief Return a list of length 4 containing \p x1, \p x2, \p x3, \p x4
 */
PMT_API pmt_t list4(const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4);

/*!
 * \brief Return a list of length 5 containing \p x1, \p x2, \p x3, \p x4, \p x5
 */
PMT_API pmt_t list5(
    const pmt_t& x1, const pmt_t& x2, const pmt_t& x3, const pmt_t& x4, const pmt_t& x5);

/*!
 * \brief Return a list of length 6 containing \p x1, \p x2, \p x3, \p x4, \p
 * x5, \p x6
 */
PMT_API pmt_t list6(const pmt_t& x1,
                    const pmt_t& x2,
                    const pmt_t& x3,
                    const pmt_t& x4,
                    const pmt_t& x5,
                    const pmt_t& x6);

/*!
 * \brief Return \p list with \p item added to it.
 */
PMT_API pmt_t list_add(pmt_t list, const pmt_t& item);

/*!
 * \brief Return \p list with \p item removed from it.
 */
PMT_API pmt_t list_rm(pmt_t list, const pmt_t& item);

/*!
 * \brief Return bool of \p list contains \p item
 */
PMT_API bool list_has(pmt_t list, const pmt_t& item);


/*
 * ------------------------------------------------------------------------
 *			     read / write
 * ------------------------------------------------------------------------
 */

//! return true if obj is the EOF object, otherwise return false.
PMT_API bool is_eof_object(pmt_t obj);

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
PMT_API pmt_t read(std::istream& port);

/*!
 * Write a written representation of \p obj to the given \p port.
 */
PMT_API void write(pmt_t obj, std::ostream& port);

/*!
 * Return a string representation of \p obj.
 * This is the same output as would be generated by pmt::write.
 */
PMT_API std::string write_string(pmt_t obj);


PMT_API std::ostream& operator<<(std::ostream& os, pmt_t obj);

/*!
 * \brief Write pmt string representation to stdout.
 */
PMT_API void print(pmt_t v);


/*
 * ------------------------------------------------------------------------
 *		      portable byte stream representation
 * ------------------------------------------------------------------------
 */
/*!
 * \brief Write portable byte-serial representation of \p obj to \p sink
 */
PMT_API bool serialize(pmt_t obj, std::streambuf& sink);

/*!
 * \brief Create obj from portable byte-serial representation
 */
PMT_API pmt_t deserialize(std::streambuf& source);


PMT_API void dump_sizeof(); // debugging

/*!
 * \brief Provide a simple string generating interface to pmt's serialize function
 */
PMT_API std::string serialize_str(pmt_t obj);

/*!
 * \brief Provide a simple string generating interface to pmt's deserialize function
 */
PMT_API pmt_t deserialize_str(std::string str);

/*!
 * \brief Provide a comparator function object to allow pmt use in stl types
 */
class comparator
{
public:
    bool operator()(pmt::pmt_t const& p1, pmt::pmt_t const& p2) const
    {
        return pmt::eqv(p1, p2) ? false : p1.get() > p2.get();
    }
};

} /* namespace pmt */

#include <pmt/pmt_sugar.h>

#endif /* INCLUDED_PMT_H */

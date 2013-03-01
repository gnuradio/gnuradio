/* -*- c++ -*- */
/*
 * Copyright 2007,2009 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vector>
#include <gruel/pmt.h>
#include "pmt_int.h"
#include "gruel/pmt_serial_tags.h"

namespace pmt {

static pmt_t parse_pair(std::streambuf &sb);

// ----------------------------------------------------------------
// output primitives
// ----------------------------------------------------------------

static bool
serialize_untagged_u8(unsigned int i, std::streambuf &sb)
{
  return sb.sputc((i >> 0) & 0xff) != std::streambuf::traits_type::eof();
}

// always writes big-endian
static bool
serialize_untagged_u16(unsigned int i, std::streambuf &sb)
{
  sb.sputc((i >> 8) & 0xff);
  return sb.sputc((i >> 0) & 0xff) != std::streambuf::traits_type::eof();
}

// always writes big-endian
static bool
serialize_untagged_u32(unsigned int i, std::streambuf &sb)
{
  sb.sputc((i >> 24) & 0xff);
  sb.sputc((i >> 16) & 0xff);
  sb.sputc((i >>  8) & 0xff);
  return sb.sputc((i >> 0) & 0xff) != std::streambuf::traits_type::eof();
}

static bool
serialize_untagged_f64(double i, std::streambuf &sb)
{
 typedef union {
	double id;
	uint64_t ii;
	} iu_t;
  iu_t iu;
  iu.id = i;
  sb.sputc((iu.ii >> 56) & 0xff);
  sb.sputc((iu.ii >> 48) & 0xff);
  sb.sputc((iu.ii >> 40) & 0xff);
  sb.sputc((iu.ii >> 32) & 0xff);
  sb.sputc((iu.ii >> 24) & 0xff);
  sb.sputc((iu.ii >> 16) & 0xff);
  sb.sputc((iu.ii >>  8) & 0xff);
  return sb.sputc((iu.ii >>  0) & 0xff) != std::streambuf::traits_type::eof();
}


// always writes big-endian
static bool
serialize_untagged_u64(uint64_t i, std::streambuf &sb)
{
  sb.sputc((i >> 56) & 0xff);
  sb.sputc((i >> 48) & 0xff);
  sb.sputc((i >> 40) & 0xff);
  sb.sputc((i >> 32) & 0xff);
  sb.sputc((i >> 24) & 0xff);
  sb.sputc((i >> 16) & 0xff);
  sb.sputc((i >>  8) & 0xff);
  return sb.sputc((i >> 0) & 0xff) != std::streambuf::traits_type::eof();
}

// ----------------------------------------------------------------
// input primitives
// ----------------------------------------------------------------


// always reads big-endian
static bool
deserialize_untagged_u8(uint8_t *ip, std::streambuf &sb)
{
  std::streambuf::traits_type::int_type  t;
  int i;

  t = sb.sbumpc();
  i = t & 0xff;

  *ip = i;
  return t != std::streambuf::traits_type::eof();
}

// always reads big-endian
static bool
deserialize_untagged_u16(uint16_t *ip, std::streambuf &sb)
{
  std::streambuf::traits_type::int_type  t;
  int i;

  t = sb.sbumpc();
  i = t & 0xff;

  t = sb.sbumpc();
  i = (i << 8) | (t & 0xff);

  *ip = i;
  return t != std::streambuf::traits_type::eof();
}

// always reads big-endian
static bool
deserialize_untagged_u32(uint32_t *ip, std::streambuf &sb)
{
  std::streambuf::traits_type::int_type  t;
  int i;

  t = sb.sbumpc();
  i = t & 0xff;

  t = sb.sbumpc();
  i = (i << 8) | (t & 0xff);
  t = sb.sbumpc();
  i = (i << 8) | (t & 0xff);
  t = sb.sbumpc();
  i = (i << 8) | (t & 0xff);

  *ip = i;
  return t != std::streambuf::traits_type::eof();
}

// always reads big-endian
static bool
deserialize_untagged_u64(uint64_t *ip, std::streambuf &sb)
{
  std::streambuf::traits_type::int_type  t;
  uint64_t i;

  t = sb.sbumpc();
  i = t & 0xff;

  t = sb.sbumpc();
  i = (i << 8) | (t & 0xff);
  t = sb.sbumpc();
  i = (i << 8) | (t & 0xff);
  t = sb.sbumpc();
  i = (i << 8) | (t & 0xff);
  t = sb.sbumpc();
  i = (i << 8) | (t & 0xff);
  t = sb.sbumpc();
  i = (i << 8) | (t & 0xff);
  t = sb.sbumpc();
  i = (i << 8) | (t & 0xff);
  t = sb.sbumpc();
  i = (i << 8) | (t & 0xff);

  *ip = i;
  return t != std::streambuf::traits_type::eof();
}

static bool
deserialize_untagged_f64(double *ip, std::streambuf &sb)
{
  std::streambuf::traits_type::int_type t;

  typedef union {
    double id;
    uint64_t ii;
  } iu_t;

  iu_t iu;

  t = sb.sbumpc();
  iu.ii = t & 0xff;

  t = sb.sbumpc();
  iu.ii = (iu.ii<<8) | (t & 0xff);
  t = sb.sbumpc();
  iu.ii = (iu.ii<<8) | (t & 0xff);
  t = sb.sbumpc();
  iu.ii = (iu.ii<<8) | (t & 0xff);
  t = sb.sbumpc();
  iu.ii = (iu.ii<<8) | (t & 0xff);
  t = sb.sbumpc();
  iu.ii = (iu.ii<<8) | (t & 0xff);
  t = sb.sbumpc();
  iu.ii = (iu.ii<<8) | (t & 0xff);
  t = sb.sbumpc();
  iu.ii = (iu.ii<<8) | (t & 0xff);

  *ip = iu.id;
  return t != std::streambuf::traits_type::eof();
}

static bool
deserialize_tuple(pmt_t *tuple, std::streambuf &sb)
{
    uint32_t nitems;
    bool ok = deserialize_untagged_u32(&nitems, sb);
    pmt_t list(PMT_NIL);
    for(uint32_t i=0; i<nitems; i++){   
        pmt_t item = pmt_deserialize( sb );
        if(pmt_eq(list, PMT_NIL)){
            list = pmt_list1(item);
        } else {
            list = pmt_list_add(list, item);
        }
    }
    (*tuple) = pmt_to_tuple(list);
    return ok;
}


/*
 * Write portable byte-serial representation of \p obj to \p sb
 *
 * N.B., Circular structures cause infinite recursion.
 */
bool
pmt_serialize(pmt_t obj, std::streambuf &sb)
{
  bool ok = true;

 tail_recursion:

  if (pmt_is_bool(obj)){
    if (pmt_eq(obj, PMT_T))
      return serialize_untagged_u8(PST_TRUE, sb);
    else
      return serialize_untagged_u8(PST_FALSE, sb);
  }

  if (pmt_is_null(obj))
    return serialize_untagged_u8(PST_NULL, sb);

  if (pmt_is_symbol(obj)){
    const std::string s = pmt_symbol_to_string(obj);
    size_t len = s.size();
    ok = serialize_untagged_u8(PST_SYMBOL, sb);
    ok &= serialize_untagged_u16(len, sb);
    for (size_t i = 0; i < len; i++)
      ok &= serialize_untagged_u8(s[i], sb);
    return ok;
  }

  if (pmt_is_pair(obj)){
    ok = serialize_untagged_u8(PST_PAIR, sb);
    ok &= pmt_serialize(pmt_car(obj), sb);
    if (!ok)
      return false;
    obj = pmt_cdr(obj);
    goto tail_recursion;
  }

  if (pmt_is_number(obj)){
    
    if (pmt_is_uint64(obj)){
        uint64_t i = pmt_to_uint64(obj);
        ok = serialize_untagged_u8(PST_UINT64, sb);
        ok &= serialize_untagged_u64(i, sb);
        return ok;
        } else 
    if (pmt_is_integer(obj)){
      long i = pmt_to_long(obj);
      if (sizeof(long) > 4){
	if (i < -2147483647 || i > 2147483647)
	  throw pmt_notimplemented("pmt_serialize (64-bit integers)", obj);
      }
      ok = serialize_untagged_u8(PST_INT32, sb);
      ok &= serialize_untagged_u32(i, sb);
      return ok;
    }

    if (pmt_is_real(obj)){
      float i = pmt_to_double(obj);
      ok = serialize_untagged_u8(PST_DOUBLE, sb);
      ok &= serialize_untagged_f64(i, sb);
      return ok;
    }

    if (pmt_is_complex(obj)){
      std::complex<double> i = pmt_to_complex(obj);
      ok = serialize_untagged_u8(PST_COMPLEX, sb);
      ok &= serialize_untagged_f64(i.real(), sb);
      ok &= serialize_untagged_f64(i.imag(), sb);
      return ok;
    }

  }

  if (pmt_is_vector(obj)) {
    size_t vec_len = pmt::pmt_length(obj);
    ok = serialize_untagged_u8(PST_VECTOR, sb);
    ok &= serialize_untagged_u32(vec_len, sb);
    for(size_t i=0; i<vec_len; i++) {
      ok &= pmt_serialize(pmt_vector_ref(obj, i), sb);
    }
    return ok;
  }

  if (pmt_is_uniform_vector(obj)) {
    size_t npad = 1;
    size_t vec_len = pmt::pmt_length(obj);

    if(pmt_is_u8vector(obj)) {
      ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
      ok &= serialize_untagged_u8(UVI_U8, sb);
      ok &= serialize_untagged_u32(vec_len, sb);
      ok &= serialize_untagged_u8(npad, sb);
      for(size_t i=0; i<npad; i++) {
	ok &= serialize_untagged_u8(0, sb);
      }
      for(size_t i=0; i<vec_len; i++) {
	ok &= serialize_untagged_u8(pmt_u8vector_ref(obj, i), sb);
      }
      return ok;
    }

    if(pmt_is_s8vector(obj)) {
      ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
      ok &= serialize_untagged_u8(UVI_S8, sb);
      ok &= serialize_untagged_u32(vec_len, sb);
      ok &= serialize_untagged_u8(npad, sb);
      for(size_t i=0; i<npad; i++) {
	ok &= serialize_untagged_u8(0, sb);
      }
      for(size_t i=0; i<vec_len; i++) {
	ok &= serialize_untagged_u8(pmt_s8vector_ref(obj, i), sb);
      }
      return ok;
    }

    if(pmt_is_u16vector(obj)) {
      ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
      ok &= serialize_untagged_u8(UVI_U16, sb);
      ok &= serialize_untagged_u32(vec_len, sb);
      ok &= serialize_untagged_u8(npad, sb);
      for(size_t i=0; i<npad; i++) {
	ok &= serialize_untagged_u8(0, sb);
      }
      for(size_t i=0; i<vec_len; i++) {
	ok &= serialize_untagged_u16(pmt_u16vector_ref(obj, i), sb);
      }
      return ok;
    }

    if(pmt_is_s16vector(obj)) {
      ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
      ok &= serialize_untagged_u8(UVI_S16, sb);
      ok &= serialize_untagged_u32(vec_len, sb);
      ok &= serialize_untagged_u8(npad, sb);
      for(size_t i=0; i<npad; i++) {
	ok &= serialize_untagged_u8(0, sb);
      }
      for(size_t i=0; i<vec_len; i++) {
	ok &= serialize_untagged_u16(pmt_s16vector_ref(obj, i), sb);
      }
      return ok;
    }

    if(pmt_is_u32vector(obj)) {
      ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
      ok &= serialize_untagged_u8(UVI_U32, sb);
      ok &= serialize_untagged_u32(vec_len, sb);
      ok &= serialize_untagged_u8(npad, sb);
      for(size_t i=0; i<npad; i++) {
	ok &= serialize_untagged_u8(0, sb);
      }
      for(size_t i=0; i<vec_len; i++) {
	ok &= serialize_untagged_u32(pmt_u32vector_ref(obj, i), sb);
      }
      return ok;
    }

    if(pmt_is_s32vector(obj)) {
      ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
      ok &= serialize_untagged_u8(UVI_S32, sb);
      ok &= serialize_untagged_u32(vec_len, sb);
      ok &= serialize_untagged_u8(npad, sb);
      for(size_t i=0; i<npad; i++) {
	ok &= serialize_untagged_u8(0, sb);
      }
      for(size_t i=0; i<vec_len; i++) {
	ok &= serialize_untagged_u32(pmt_s32vector_ref(obj, i), sb);
      }
      return ok;
    }

    if(pmt_is_u64vector(obj)) {
      ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
      ok &= serialize_untagged_u8(UVI_U64, sb);
      ok &= serialize_untagged_u32(vec_len, sb);
      ok &= serialize_untagged_u8(npad, sb);
      for(size_t i=0; i<npad; i++) {
	ok &= serialize_untagged_u8(0, sb);
      }
      for(size_t i=0; i<vec_len; i++) {
	ok &= serialize_untagged_u64(pmt_u64vector_ref(obj, i), sb);
      }
      return ok;
    }

    if(pmt_is_s64vector(obj)) {
      ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
      ok &= serialize_untagged_u8(UVI_S64, sb);
      ok &= serialize_untagged_u32(vec_len, sb);
      ok &= serialize_untagged_u8(npad, sb);
      for(size_t i=0; i<npad; i++) {
	ok &= serialize_untagged_u8(0, sb);
      }
      for(size_t i=0; i<vec_len; i++) {
	ok &= serialize_untagged_u64(pmt_s64vector_ref(obj, i), sb);
      }
      return ok;
    }

    if(pmt_is_f32vector(obj)) {
      ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
      ok &= serialize_untagged_u8(UVI_F32, sb);
      ok &= serialize_untagged_u32(vec_len, sb);
      ok &= serialize_untagged_u8(npad, sb);
      for(size_t i=0; i<npad; i++) {
	ok &= serialize_untagged_u8(0, sb);
      }
      for(size_t i=0; i<vec_len; i++) {
	ok &= serialize_untagged_f64(pmt_f32vector_ref(obj, i), sb);
      }
      return ok;
    }

    if(pmt_is_f64vector(obj)) {
      ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
      ok &= serialize_untagged_u8(UVI_F64, sb);
      ok &= serialize_untagged_u32(vec_len, sb);
      ok &= serialize_untagged_u8(npad, sb);
      for(size_t i=0; i<npad; i++) {
	ok &= serialize_untagged_u8(0, sb);
      }
      for(size_t i=0; i<vec_len; i++) {
	ok &= serialize_untagged_f64(pmt_f64vector_ref(obj, i), sb);
      }
      return ok;
    }

    if(pmt_is_c32vector(obj)) {
      ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
      ok &= serialize_untagged_u8(UVI_C32, sb);
      ok &= serialize_untagged_u32(vec_len, sb);
      ok &= serialize_untagged_u8(npad, sb);
      for(size_t i=0; i<npad; i++) {
	ok &= serialize_untagged_u8(0, sb);
      }
      for(size_t i=0; i<vec_len; i++) {
	std::complex<float> c = pmt_c32vector_ref(obj, i);
	ok &= serialize_untagged_f64(c.real(), sb);
	ok &= serialize_untagged_f64(c.imag(), sb);
      }
      return ok;
    }

    if(pmt_is_c64vector(obj)) {
      ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
      ok &= serialize_untagged_u8(UVI_C64, sb);
      ok &= serialize_untagged_u32(vec_len, sb);
      ok &= serialize_untagged_u8(npad, sb);
      for(size_t i=0; i<npad; i++) {
	ok &= serialize_untagged_u8(0, sb);
      }
      for(size_t i=0; i<vec_len; i++) {
	std::complex<double> c = pmt_c64vector_ref(obj, i);
	ok &= serialize_untagged_f64(c.real(), sb);
	ok &= serialize_untagged_f64(c.imag(), sb);
      }
      return ok;
    }
  }

  if (pmt_is_dict(obj))
    throw pmt_notimplemented("pmt_serialize (dict)", obj);

  if (pmt_is_tuple(obj)){
    size_t tuple_len = pmt::pmt_length(obj);
    ok = serialize_untagged_u8(PST_TUPLE, sb);
    ok &= serialize_untagged_u32(tuple_len, sb);
    for(size_t i=0; i<tuple_len; i++){
        ok &= pmt_serialize(pmt_tuple_ref(obj, i), sb);
    }
    return ok;
  }
  //throw pmt_notimplemented("pmt_serialize (tuple)", obj);

  throw pmt_notimplemented("pmt_serialize (?)", obj);
}

/*
 * Create obj from portable byte-serial representation
 *
 * Returns next obj from streambuf, or PMT_EOF at end of file.
 * Throws exception on malformed input.
 */
pmt_t
pmt_deserialize(std::streambuf &sb)
{
  uint8_t	tag;
  uint8_t	u8;
  uint16_t	u16;
  uint32_t	u32;
  uint64_t	u64;
  double	f64;
  static char   tmpbuf[1024];

  if (!deserialize_untagged_u8(&tag, sb))
    return PMT_EOF;

  switch (tag){
  case PST_TRUE:
    return PMT_T;

  case PST_FALSE:
    return PMT_F;

  case PST_NULL:
    return PMT_NIL;

  case PST_SYMBOL:
    if (!deserialize_untagged_u16(&u16, sb))
      goto error;
    if (u16 > sizeof(tmpbuf))
      throw pmt_notimplemented("pmt_deserialize: very long symbol",
			       PMT_F);
    if (sb.sgetn(tmpbuf, u16) != u16)
      goto error;
    return pmt_intern(std::string(tmpbuf, u16));

  case PST_INT32:
    if (!deserialize_untagged_u32(&u32, sb))
      goto error;
    return pmt_from_long((int32_t) u32);

  case PST_UINT64:
    if(!deserialize_untagged_u64(&u64, sb))
        goto error;
    return pmt_from_uint64(u64);

  case PST_PAIR:
    return parse_pair(sb);

  case PST_DOUBLE:
    if(!deserialize_untagged_f64(&f64, sb))
      goto error;
    return pmt_from_double( f64 );

  case PST_COMPLEX:
    {
    double r,i;
    if(!deserialize_untagged_f64(&r, sb) && !deserialize_untagged_f64(&i, sb))
      goto error;
    return pmt_make_rectangular( r,i );
    }
    
  case PST_TUPLE:
    {
    pmt_t tuple;
    if(!deserialize_tuple(&tuple, sb)){
      goto error;
    }
    return tuple;
    }

  case PST_VECTOR:
    {
    uint32_t nitems;
    if(!deserialize_untagged_u32(&nitems, sb))
      goto error;
    pmt_t vec = pmt_make_vector(nitems, PMT_NIL);
    for(uint32_t i=0; i<nitems; i++) {
      pmt_t item = pmt_deserialize(sb);
      pmt_vector_set(vec, i, item);
    }
    return vec;
    }

  case PST_UNIFORM_VECTOR:
    {
      uint8_t utag, npad;
      uint32_t nitems;

      if(!deserialize_untagged_u8(&utag, sb))
	return PMT_EOF;

      if(!deserialize_untagged_u32(&nitems, sb))
	goto error;
      
      deserialize_untagged_u8(&npad, sb);
      for(size_t i; i < npad; i++)
	deserialize_untagged_u8(&u8, sb);

      switch(utag) {
      case(UVI_U8):
	{
	  pmt_t vec = pmt_make_u8vector(nitems, 0);
	  for(uint32_t i=0; i<nitems; i++) {
	    deserialize_untagged_u8(&u8, sb);
	    pmt_u8vector_set(vec, i, u8);
	  }
	  return vec;
	}
      case(UVI_S8):
	{
	  pmt_t vec = pmt_make_s8vector(nitems, 0);
	  for(uint32_t i=0; i<nitems; i++) {
	    deserialize_untagged_u8(&u8, sb);
	    pmt_s8vector_set(vec, i, u8);
	  }
	  return vec;
	}
      case(UVI_U16):
	{
	  pmt_t vec = pmt_make_u16vector(nitems, 0);
	  for(uint32_t i=0; i<nitems; i++) {
	    deserialize_untagged_u16(&u16, sb);
	    pmt_u16vector_set(vec, i, u16);
	  }
	  return vec;
	}
      case(UVI_S16):
	{
	  pmt_t vec = pmt_make_s16vector(nitems, 0);
	  for(uint32_t i=0; i<nitems; i++) {
	    deserialize_untagged_u16(&u16, sb);
	    pmt_s16vector_set(vec, i, u16);
	  }
	  return vec;
	}
      case(UVI_U32):
	{
	  pmt_t vec = pmt_make_u32vector(nitems, 0);
	  for(uint32_t i=0; i<nitems; i++) {
	    deserialize_untagged_u32(&u32, sb);
	    pmt_u32vector_set(vec, i, u32);
	  }
	  return vec;
	}
      case(UVI_S32):
	{
	  pmt_t vec = pmt_make_s32vector(nitems, 0);
	  for(uint32_t i=0; i<nitems; i++) {
	    deserialize_untagged_u32(&u32, sb);
	    pmt_s32vector_set(vec, i, u32);
	  }
	  return vec;
	}
      case(UVI_U64):
	{
	  pmt_t vec = pmt_make_u64vector(nitems, 0);
	  for(uint32_t i=0; i<nitems; i++) {
	    deserialize_untagged_u64(&u64, sb);
	    pmt_u64vector_set(vec, i, u64);
	  }
	  return vec;
	}
      case(UVI_S64):
	{
	  pmt_t vec = pmt_make_s64vector(nitems, 0);
	  for(uint32_t i=0; i<nitems; i++) {
	    deserialize_untagged_u64(&u64, sb);
	    pmt_s64vector_set(vec, i, u64);
	  }
	  return vec;
	}
      case(UVI_F32):
	{
	  pmt_t vec = pmt_make_f32vector(nitems, 0);
	  for(uint32_t i=0; i<nitems; i++) {
	    deserialize_untagged_f64(&f64, sb);
	    pmt_f32vector_set(vec, i, static_cast<float>(f64));
	  }
	  return vec;
	}
      case(UVI_F64):
	{
	  pmt_t vec = pmt_make_f64vector(nitems, 0);
	  for(uint32_t i=0; i<nitems; i++) {
	    deserialize_untagged_f64(&f64, sb);
	    pmt_f64vector_set(vec, i, f64);
	  }
	  return vec;
	}
      case(UVI_C32):
	{
	  pmt_t vec = pmt_make_c32vector(nitems, 0);
	  for(uint32_t i=0; i<nitems; i++) {
	    float re, im;
	    deserialize_untagged_f64(&f64, sb);
	    re = static_cast<float>(f64);
	    deserialize_untagged_f64(&f64, sb);
	    im = static_cast<float>(f64);
	    pmt_c32vector_set(vec, i, std::complex<float>(re, im));
	  }
	  return vec;
	}

      case(UVI_C64):
	{
	  pmt_t vec = pmt_make_c64vector(nitems, 0);
	  for(uint32_t i=0; i<nitems; i++) {
	    double re, im;
	    deserialize_untagged_f64(&f64, sb);
	    re = f64;
	    deserialize_untagged_f64(&f64, sb);
	    im = f64;
	    pmt_c64vector_set(vec, i, std::complex<double>(re, im));
	  }
	  return vec;
	}

      default:
	throw pmt_exception("pmt_deserialize: malformed input stream, tag value = ",
			    pmt_from_long(tag));
      }
    }

  case PST_DICT:
  case PST_COMMENT:
    throw pmt_notimplemented("pmt_deserialize: tag value = ",
			     pmt_from_long(tag));

  default:
    throw pmt_exception("pmt_deserialize: malformed input stream, tag value = ",
			pmt_from_long(tag));
  }

 error:
  throw pmt_exception("pmt_deserialize: malformed input stream", PMT_F);
}


/*
 * provide a simple string accessor to the serialized pmt form
 */
std::string pmt_serialize_str(pmt_t obj){
  std::stringbuf sb;
  pmt_serialize(obj, sb);
  return sb.str();
}


/*
 * provide a simple string accessor to the deserialized pmt form
 */
pmt_t pmt_deserialize_str(std::string s){
  std::stringbuf sb(s);
  return pmt_deserialize(sb);
}


/*
 * This is a mostly non-recursive implementation that allows us to
 * deserialize very long lists w/o exhausting the evaluation stack.
 *
 * On entry we've already eaten the PST_PAIR tag.
 */
pmt_t
parse_pair(std::streambuf &sb)
{
  uint8_t tag;
  pmt_t	val, expr, lastnptr, nptr;

  //
  // Keep appending nodes until we get a non-PAIR cdr.
  //
  lastnptr = PMT_NIL;
  while (1){
    expr = pmt_deserialize(sb);		// read the car

    nptr = pmt_cons(expr, PMT_NIL);	// build new cell
    if (pmt_is_null(lastnptr))
      val = nptr;
    else
      pmt_set_cdr(lastnptr, nptr);
    lastnptr = nptr;

    if (!deserialize_untagged_u8(&tag, sb))  // get tag of cdr
      throw pmt_exception("pmt_deserialize: malformed input stream", PMT_F);

    if (tag == PST_PAIR)
      continue;			// keep on looping...

    if (tag == PST_NULL){
      expr = PMT_NIL;
      break;
    }

    //
    // default: push tag back and use pmt_deserialize to get the cdr
    //
    sb.sungetc();
    expr = pmt_deserialize(sb);
    break;
  }

  //
  // At this point, expr contains the value of the final cdr in the list.
  //
  pmt_set_cdr(lastnptr, expr);
  return val;
}

} /* namespace pmt */

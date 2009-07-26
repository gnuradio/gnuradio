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

#if 0
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
#endif

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

#if 0
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
#endif

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

    if (pmt_is_real(obj))
      throw pmt_notimplemented("pmt_serialize (real)", obj);

    if (pmt_is_complex(obj))
      throw pmt_notimplemented("pmt_serialize (complex)", obj);
  }

  if (pmt_is_vector(obj))
    throw pmt_notimplemented("pmt_serialize (vector)", obj);

  if (pmt_is_uniform_vector(obj))
    throw pmt_notimplemented("pmt_serialize (uniform-vector)", obj);
    
  if (pmt_is_dict(obj))
    throw pmt_notimplemented("pmt_serialize (dict)", obj);
    

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
  //uint8_t	u8;
  uint16_t	u16;
  uint32_t	u32;
  //uint32_t	u64;
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

  case PST_PAIR:
    return parse_pair(sb);

  case PST_DOUBLE:
  case PST_COMPLEX:
  case PST_VECTOR:
  case PST_DICT:
  case PST_UNIFORM_VECTOR:
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

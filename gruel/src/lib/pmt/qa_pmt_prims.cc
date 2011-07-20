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

#include <qa_pmt_prims.h>
#include <cppunit/TestAssert.h>
#include <gruel/msg_passing.h>
#include <boost/format.hpp>
#include <cstdio>
#include <cstring>
#include <sstream>

using namespace pmt;

void
qa_pmt_prims::test_symbols()
{
  CPPUNIT_ASSERT(!pmt_is_symbol(PMT_T));
  CPPUNIT_ASSERT(!pmt_is_symbol(PMT_F));
  CPPUNIT_ASSERT_THROW(pmt_symbol_to_string(PMT_F), pmt_wrong_type);

  pmt_t sym1 = mp("test");
  CPPUNIT_ASSERT(pmt_is_symbol(sym1));
  CPPUNIT_ASSERT_EQUAL(std::string("test"), pmt_symbol_to_string(sym1));
  CPPUNIT_ASSERT(pmt_is_true(sym1));
  CPPUNIT_ASSERT(!pmt_is_false(sym1));

  pmt_t sym2 = mp("foo");
  pmt_t sym3 = mp("test");
  CPPUNIT_ASSERT_EQUAL(sym1, sym3);
  CPPUNIT_ASSERT(sym1 != sym2);
  CPPUNIT_ASSERT(sym1 == sym3);

  static const int N = 2048;
  std::vector<pmt_t> v1(N);
  std::vector<pmt_t> v2(N);

  // generate a bunch of symbols
  for (int i = 0; i < N; i++){
    std::string buf = str(boost::format("test-%d") % i);
    v1[i] = mp(buf.c_str());
  }

  // confirm that they are all unique
  for (int i = 0; i < N; i++)
    for (int j = i + 1; j < N; j++)
      CPPUNIT_ASSERT(v1[i] != v1[j]);

  // generate the same symbols again
  for (int i = 0; i < N; i++){
    std::string buf = str(boost::format("test-%d") % i);
    v2[i] = mp(buf.c_str());
  }

  // confirm that we get the same ones back
  for (int i = 0; i < N; i++)
    CPPUNIT_ASSERT(v1[i] == v2[i]);
}

void
qa_pmt_prims::test_booleans()
{
  pmt_t sym = mp("test");
  CPPUNIT_ASSERT(pmt_is_bool(PMT_T));
  CPPUNIT_ASSERT(pmt_is_bool(PMT_F));
  CPPUNIT_ASSERT(!pmt_is_bool(sym));
  CPPUNIT_ASSERT_EQUAL(pmt_from_bool(false), PMT_F);
  CPPUNIT_ASSERT_EQUAL(pmt_from_bool(true), PMT_T);
  CPPUNIT_ASSERT_EQUAL(false, pmt_to_bool(PMT_F));
  CPPUNIT_ASSERT_EQUAL(true, pmt_to_bool(PMT_T));
  CPPUNIT_ASSERT_THROW(pmt_to_bool(sym), pmt_wrong_type);
}

void
qa_pmt_prims::test_integers()
{
  pmt_t p1 = pmt_from_long(1);
  pmt_t m1 = pmt_from_long(-1);
  CPPUNIT_ASSERT(!pmt_is_integer(PMT_T));
  CPPUNIT_ASSERT(pmt_is_integer(p1));
  CPPUNIT_ASSERT(pmt_is_integer(m1));
  CPPUNIT_ASSERT_THROW(pmt_to_long(PMT_T), pmt_wrong_type);
  CPPUNIT_ASSERT_EQUAL(-1L, pmt_to_long(m1));
  CPPUNIT_ASSERT_EQUAL(1L, pmt_to_long(p1));
}

void
qa_pmt_prims::test_uint64s()
{
  pmt_t p1 = pmt_from_uint64((uint64_t)1);
  pmt_t m1 = pmt_from_uint64((uint64_t)8589934592ULL);
  CPPUNIT_ASSERT(!pmt_is_uint64(PMT_T));
  CPPUNIT_ASSERT(pmt_is_uint64(p1));
  CPPUNIT_ASSERT(pmt_is_uint64(m1));
  CPPUNIT_ASSERT_THROW(pmt_to_uint64(PMT_T), pmt_wrong_type);
  CPPUNIT_ASSERT_EQUAL((uint64_t)8589934592ULL, (uint64_t)pmt_to_uint64(m1));
  CPPUNIT_ASSERT_EQUAL((uint64_t)1ULL, (uint64_t)pmt_to_uint64(p1));
}

void
qa_pmt_prims::test_reals()
{
  pmt_t p1 = pmt_from_double(1);
  pmt_t m1 = pmt_from_double(-1);
  CPPUNIT_ASSERT(!pmt_is_real(PMT_T));
  CPPUNIT_ASSERT(pmt_is_real(p1));
  CPPUNIT_ASSERT(pmt_is_real(m1));
  CPPUNIT_ASSERT_THROW(pmt_to_double(PMT_T), pmt_wrong_type);
  CPPUNIT_ASSERT_EQUAL(-1.0, pmt_to_double(m1));
  CPPUNIT_ASSERT_EQUAL(1.0, pmt_to_double(p1));
  CPPUNIT_ASSERT_EQUAL(1.0, pmt_to_double(pmt_from_long(1)));
}

void
qa_pmt_prims::test_complexes()
{
  pmt_t p1 = pmt_make_rectangular(2, -3);
  pmt_t m1 = pmt_make_rectangular(-3, 2);
  CPPUNIT_ASSERT(!pmt_is_complex(PMT_T));
  CPPUNIT_ASSERT(pmt_is_complex(p1));
  CPPUNIT_ASSERT(pmt_is_complex(m1));
  CPPUNIT_ASSERT_THROW(pmt_to_complex(PMT_T), pmt_wrong_type);
  CPPUNIT_ASSERT_EQUAL(std::complex<double>(2, -3), pmt_to_complex(p1));
  CPPUNIT_ASSERT_EQUAL(std::complex<double>(-3, 2), pmt_to_complex(m1));
  CPPUNIT_ASSERT_EQUAL(std::complex<double>(1.0, 0), pmt_to_complex(pmt_from_long(1)));
  CPPUNIT_ASSERT_EQUAL(std::complex<double>(1.0, 0), pmt_to_complex(pmt_from_double(1.0)));
}

void
qa_pmt_prims::test_pairs()
{
  CPPUNIT_ASSERT(pmt_is_null(PMT_NIL));
  CPPUNIT_ASSERT(!pmt_is_pair(PMT_NIL));
  pmt_t s1 = mp("s1");
  pmt_t s2 = mp("s2");
  pmt_t s3 = mp("s3");


  CPPUNIT_ASSERT_EQUAL((size_t)0, pmt_length(PMT_NIL));
  CPPUNIT_ASSERT_THROW(pmt_length(s1), pmt_wrong_type);
  CPPUNIT_ASSERT_THROW(pmt_length(pmt_from_double(42)), pmt_wrong_type);

  pmt_t c1 = pmt_cons(s1, PMT_NIL);
  CPPUNIT_ASSERT(pmt_is_pair(c1));
  CPPUNIT_ASSERT(!pmt_is_pair(s1));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_car(c1));
  CPPUNIT_ASSERT_EQUAL(PMT_NIL, pmt_cdr(c1));
  CPPUNIT_ASSERT_EQUAL((size_t) 1, pmt_length(c1));

  pmt_t c3 = pmt_cons(s3, PMT_NIL);
  pmt_t c2 = pmt_cons(s2, c3);
  pmt_set_cdr(c1, c2);
  CPPUNIT_ASSERT_EQUAL(c2, pmt_cdr(c1));
  pmt_set_car(c1, s3);
  CPPUNIT_ASSERT_EQUAL(s3, pmt_car(c1));
  CPPUNIT_ASSERT_EQUAL((size_t)1, pmt_length(c3));
  CPPUNIT_ASSERT_EQUAL((size_t)2, pmt_length(c2));
  
  CPPUNIT_ASSERT_THROW(pmt_cdr(PMT_NIL), pmt_wrong_type);
  CPPUNIT_ASSERT_THROW(pmt_car(PMT_NIL), pmt_wrong_type);
  CPPUNIT_ASSERT_THROW(pmt_set_car(s1, PMT_NIL), pmt_wrong_type);
  CPPUNIT_ASSERT_THROW(pmt_set_cdr(s1, PMT_NIL), pmt_wrong_type);
}

void
qa_pmt_prims::test_vectors()
{
  static const size_t N = 3;
  pmt_t v1 = pmt_make_vector(N, PMT_NIL);
  CPPUNIT_ASSERT_EQUAL(N, pmt_length(v1));
  pmt_t s0 = mp("s0");
  pmt_t s1 = mp("s1");
  pmt_t s2 = mp("s2");

  pmt_vector_set(v1, 0, s0);
  pmt_vector_set(v1, 1, s1);
  pmt_vector_set(v1, 2, s2);

  CPPUNIT_ASSERT_EQUAL(s0, pmt_vector_ref(v1, 0));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_vector_ref(v1, 1));
  CPPUNIT_ASSERT_EQUAL(s2, pmt_vector_ref(v1, 2));

  CPPUNIT_ASSERT_THROW(pmt_vector_ref(v1, N), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_vector_set(v1, N, PMT_NIL), pmt_out_of_range);

  pmt_vector_fill(v1, s0);
  for (size_t i = 0; i < N; i++)
    CPPUNIT_ASSERT_EQUAL(s0, pmt_vector_ref(v1, i));
}

static void
check_tuple(size_t len, const std::vector<pmt_t> &s, pmt_t t)
{
  CPPUNIT_ASSERT_EQUAL(true, pmt_is_tuple(t));
  CPPUNIT_ASSERT_EQUAL(len, pmt_length(t));

  for (size_t i = 0; i < len; i++)
    CPPUNIT_ASSERT_EQUAL(s[i], pmt_tuple_ref(t, i));

}

void
qa_pmt_prims::test_tuples()
{
  pmt_t v = pmt_make_vector(10, PMT_NIL);
  std::vector<pmt_t> s(10);
  for (size_t i = 0; i < 10; i++){
    std::ostringstream os;
    os << "s" << i;
    s[i] = mp(os.str());
    pmt_vector_set(v, i, s[i]);
  }


  pmt_t t;

  t = pmt_make_tuple();
  check_tuple(0, s, t);

  t = pmt_make_tuple(s[0]);
  check_tuple(1, s, t);

  CPPUNIT_ASSERT(pmt_is_vector(v));
  CPPUNIT_ASSERT(!pmt_is_tuple(v));
  CPPUNIT_ASSERT(pmt_is_tuple(t));
  CPPUNIT_ASSERT(!pmt_is_vector(t));

  t = pmt_make_tuple(s[0], s[1]);
  check_tuple(2, s, t);

  t = pmt_make_tuple(s[0], s[1], s[2]);
  check_tuple(3, s, t);

  t = pmt_make_tuple(s[0], s[1], s[2], s[3]);
  check_tuple(4, s, t);

  t = pmt_make_tuple(s[0], s[1], s[2], s[3], s[4]);
  check_tuple(5, s, t);

  t = pmt_make_tuple(s[0], s[1], s[2], s[3], s[4], s[5]);
  check_tuple(6, s, t);

  t = pmt_make_tuple(s[0], s[1], s[2], s[3], s[4], s[5], s[6]);
  check_tuple(7, s, t);

  t = pmt_make_tuple(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7]);
  check_tuple(8, s, t);

  t = pmt_make_tuple(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8]);
  check_tuple(9, s, t);

  t = pmt_make_tuple(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8], s[9]);
  check_tuple(10, s, t);

  t = pmt_make_tuple(s[0], s[1], s[2]);
  CPPUNIT_ASSERT_THROW(pmt_tuple_ref(t, 3), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_vector_ref(t, 0), pmt_wrong_type);
  CPPUNIT_ASSERT_THROW(pmt_tuple_ref(v, 0), pmt_wrong_type);

  t = pmt_make_tuple(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8], s[9]);
  pmt_t t2 = pmt_to_tuple(v);
  CPPUNIT_ASSERT_EQUAL(size_t(10), pmt_length(v));
  CPPUNIT_ASSERT(pmt_equal(t, t2));
  //std::cout << v << std::endl;
  //std::cout << t2 << std::endl;

  
  t = pmt_make_tuple(s[0], s[1], s[2]);
  pmt_t list0 = pmt_list3(s[0], s[1], s[2]);
  CPPUNIT_ASSERT_EQUAL(size_t(3), pmt_length(list0));
  t2 = pmt_to_tuple(list0);
  CPPUNIT_ASSERT_EQUAL(size_t(3), pmt_length(t2));
  CPPUNIT_ASSERT(pmt_equal(t, t2));
}

void
qa_pmt_prims::test_equivalence()
{
  pmt_t s0 = mp("s0");
  pmt_t s1 = mp("s1");
  pmt_t s2 = mp("s2");
  pmt_t list0 = pmt_cons(s0, pmt_cons(s1, pmt_cons(s2, PMT_NIL)));
  pmt_t list1 = pmt_cons(s0, pmt_cons(s1, pmt_cons(s2, PMT_NIL)));
  pmt_t i0 = pmt_from_long(42);
  pmt_t i1 = pmt_from_long(42);
  pmt_t r0 = pmt_from_double(42);
  pmt_t r1 = pmt_from_double(42);
  pmt_t r2 = pmt_from_double(43);

  CPPUNIT_ASSERT(pmt_eq(s0, s0));
  CPPUNIT_ASSERT(!pmt_eq(s0, s1));
  CPPUNIT_ASSERT(pmt_eqv(s0, s0));
  CPPUNIT_ASSERT(!pmt_eqv(s0, s1));

  CPPUNIT_ASSERT(pmt_eqv(i0, i1));
  CPPUNIT_ASSERT(pmt_eqv(r0, r1));
  CPPUNIT_ASSERT(!pmt_eqv(r0, r2));
  CPPUNIT_ASSERT(!pmt_eqv(i0, r0));

  CPPUNIT_ASSERT(!pmt_eq(list0, list1));
  CPPUNIT_ASSERT(!pmt_eqv(list0, list1));
  CPPUNIT_ASSERT(pmt_equal(list0, list1));

  pmt_t v0 = pmt_make_vector(3, s0);
  pmt_t v1 = pmt_make_vector(3, s0);
  pmt_t v2 = pmt_make_vector(4, s0);
  CPPUNIT_ASSERT(!pmt_eqv(v0, v1));
  CPPUNIT_ASSERT(pmt_equal(v0, v1));
  CPPUNIT_ASSERT(!pmt_equal(v0, v2));

  pmt_vector_set(v0, 0, list0);
  pmt_vector_set(v0, 1, list0);
  pmt_vector_set(v1, 0, list1);
  pmt_vector_set(v1, 1, list1);
  CPPUNIT_ASSERT(pmt_equal(v0, v1));
}

void
qa_pmt_prims::test_misc()
{
  pmt_t k0 = mp("k0");
  pmt_t k1 = mp("k1");
  pmt_t k2 = mp("k2");
  pmt_t k3 = mp("k3");
  pmt_t v0 = mp("v0");
  pmt_t v1 = mp("v1");
  pmt_t v2 = mp("v2");
  pmt_t p0 = pmt_cons(k0, v0);
  pmt_t p1 = pmt_cons(k1, v1);
  pmt_t p2 = pmt_cons(k2, v2);
  
  pmt_t alist = pmt_cons(p0, pmt_cons(p1, pmt_cons(p2, PMT_NIL)));
  CPPUNIT_ASSERT(pmt_eq(p1, pmt_assv(k1, alist)));
  CPPUNIT_ASSERT(pmt_eq(PMT_F, pmt_assv(k3, alist)));
  
  pmt_t keys = pmt_cons(k0, pmt_cons(k1, pmt_cons(k2, PMT_NIL)));
  pmt_t vals = pmt_cons(v0, pmt_cons(v1, pmt_cons(v2, PMT_NIL)));
  CPPUNIT_ASSERT(pmt_equal(keys, pmt_map(pmt_car, alist)));
  CPPUNIT_ASSERT(pmt_equal(vals, pmt_map(pmt_cdr, alist)));
}

void
qa_pmt_prims::test_dict()
{
  pmt_t dict = pmt_make_dict();
  CPPUNIT_ASSERT(pmt_is_dict(dict));

  pmt_t k0 = mp("k0");
  pmt_t k1 = mp("k1");
  pmt_t k2 = mp("k2");
  pmt_t k3 = mp("k3");
  pmt_t v0 = mp("v0");
  pmt_t v1 = mp("v1");
  pmt_t v2 = mp("v2");
  pmt_t v3 = mp("v3");
  pmt_t not_found = pmt_cons(PMT_NIL, PMT_NIL);
  
  CPPUNIT_ASSERT(!pmt_dict_has_key(dict, k0));
  dict = pmt_dict_add(dict, k0, v0);
  CPPUNIT_ASSERT(pmt_dict_has_key(dict, k0));
  CPPUNIT_ASSERT(pmt_eqv(pmt_dict_ref(dict, k0, not_found), v0));
  CPPUNIT_ASSERT(pmt_eqv(pmt_dict_ref(dict, k1, not_found), not_found));
  dict = pmt_dict_add(dict, k1, v1);
  dict = pmt_dict_add(dict, k2, v2);
  CPPUNIT_ASSERT(pmt_eqv(pmt_dict_ref(dict, k1, not_found), v1));
  dict = pmt_dict_add(dict, k1, v3);
  CPPUNIT_ASSERT(pmt_eqv(pmt_dict_ref(dict, k1, not_found), v3));

  pmt_t keys = pmt_list3(k1, k2, k0);
  pmt_t vals = pmt_list3(v3, v2, v0);
  //std::cout << "pmt_dict_keys:   " << pmt_dict_keys(dict) << std::endl;
  //std::cout << "pmt_dict_values: " << pmt_dict_values(dict) << std::endl;
  CPPUNIT_ASSERT(pmt_equal(keys, pmt_dict_keys(dict)));
  CPPUNIT_ASSERT(pmt_equal(vals, pmt_dict_values(dict)));
}

void
qa_pmt_prims::test_io()
{
  pmt_t k0 = mp("k0");
  pmt_t k1 = mp("k1");
  pmt_t k2 = mp("k2");
  pmt_t k3 = mp("k3");

  CPPUNIT_ASSERT_EQUAL(std::string("k0"), pmt_write_string(k0));
}

void
qa_pmt_prims::test_lists()
{
  pmt_t s0 = mp("s0");
  pmt_t s1 = mp("s1");
  pmt_t s2 = mp("s2");
  pmt_t s3 = mp("s3");

  pmt_t l1 = pmt_list4(s0, s1, s2, s3);
  pmt_t l2 = pmt_list3(s0, s1, s2);
  pmt_t l3 = pmt_list_add(l2, s3);
  CPPUNIT_ASSERT(pmt_equal(l1, l3));
}

// ------------------------------------------------------------------------

// class foo is used in test_any below.
// It can't be declared in the scope of test_any because of template
// namespace problems.

class foo {
public:
  double	d_double;
  int		d_int;
  foo(double d=0, int i=0) : d_double(d), d_int(i) {}
};

bool operator==(const foo &a, const foo &b)
{
  return a.d_double == b.d_double && a.d_int == b.d_int;
}

std::ostream& operator<<(std::ostream &os, const foo obj)
{
  os << "<foo: " << obj.d_double << ", " << obj.d_int << ">";
  return os;
}

void
qa_pmt_prims::test_any()
{
  boost::any a0;
  boost::any a1;
  boost::any a2;

  a0 = std::string("Hello!");
  a1 = 42;
  a2 = foo(3.250, 21);

  pmt_t p0 = pmt_make_any(a0);
  pmt_t p1 = pmt_make_any(a1);
  pmt_t p2 = pmt_make_any(a2);

  CPPUNIT_ASSERT_EQUAL(std::string("Hello!"),
		       boost::any_cast<std::string>(pmt_any_ref(p0)));

  CPPUNIT_ASSERT_EQUAL(42,
		       boost::any_cast<int>(pmt_any_ref(p1)));

  CPPUNIT_ASSERT_EQUAL(foo(3.250, 21),
		       boost::any_cast<foo>(pmt_any_ref(p2)));
}

// ------------------------------------------------------------------------

class qa_pmt_msg_accepter_nop : public gruel::msg_accepter {
public:
  qa_pmt_msg_accepter_nop(){};
  ~qa_pmt_msg_accepter_nop();
  void post(pmt_t){};
};

qa_pmt_msg_accepter_nop::~qa_pmt_msg_accepter_nop(){}

void
qa_pmt_prims::test_msg_accepter()
{
  pmt_t sym = mp("my-symbol");

  boost::any a0;
  a0 = std::string("Hello!");
  pmt_t p0 = pmt_make_any(a0);

  gruel::msg_accepter_sptr ma0 = gruel::msg_accepter_sptr(new qa_pmt_msg_accepter_nop());
  pmt_t p1 = pmt_make_msg_accepter(ma0);

  CPPUNIT_ASSERT_EQUAL(ma0.get(), pmt_msg_accepter_ref(p1).get());

  CPPUNIT_ASSERT_THROW(pmt_msg_accepter_ref(sym), pmt_wrong_type);
  CPPUNIT_ASSERT_THROW(pmt_msg_accepter_ref(p0),  pmt_wrong_type);

  // just confirm interfaces on send are OK
  gruel::send(ma0.get(), sym);
  gruel::send(ma0, sym);
  gruel::send(p1, sym);

}

// ------------------------------------------------------------------------

void
qa_pmt_prims::test_serialize()
{
  std::stringbuf sb;		// fake channel
  pmt_t a = mp("a");
  pmt_t b = mp("b");
  pmt_t c = mp("c");

  sb.str("");			// reset channel to empty

  // write stuff to channel

  pmt_serialize(PMT_NIL, sb);
  pmt_serialize(mp("foobarvia"), sb);
  pmt_serialize(pmt_from_long(123456789), sb);
  pmt_serialize(pmt_from_long(-123456789), sb);
  pmt_serialize(pmt_cons(PMT_NIL, PMT_NIL), sb);
  pmt_serialize(pmt_cons(a, b), sb);
  pmt_serialize(pmt_list1(a), sb);
  pmt_serialize(pmt_list2(a, b), sb);
  pmt_serialize(pmt_list3(a, b, c), sb);
  pmt_serialize(pmt_list3(a, pmt_list3(c, b, a), c), sb);
  pmt_serialize(PMT_T, sb);
  pmt_serialize(PMT_F, sb);

  // read it back

  CPPUNIT_ASSERT(pmt_equal(pmt_deserialize(sb), PMT_NIL));
  CPPUNIT_ASSERT(pmt_equal(pmt_deserialize(sb), mp("foobarvia")));
  CPPUNIT_ASSERT(pmt_equal(pmt_deserialize(sb), pmt_from_long(123456789)));
  CPPUNIT_ASSERT(pmt_equal(pmt_deserialize(sb), pmt_from_long(-123456789)));
  CPPUNIT_ASSERT(pmt_equal(pmt_deserialize(sb), pmt_cons(PMT_NIL, PMT_NIL)));
  CPPUNIT_ASSERT(pmt_equal(pmt_deserialize(sb), pmt_cons(a, b)));
  CPPUNIT_ASSERT(pmt_equal(pmt_deserialize(sb), pmt_list1(a)));
  CPPUNIT_ASSERT(pmt_equal(pmt_deserialize(sb), pmt_list2(a, b)));
  CPPUNIT_ASSERT(pmt_equal(pmt_deserialize(sb), pmt_list3(a, b, c)));
  CPPUNIT_ASSERT(pmt_equal(pmt_deserialize(sb), pmt_list3(a, pmt_list3(c, b, a), c)));
  CPPUNIT_ASSERT(pmt_equal(pmt_deserialize(sb), PMT_T));
  CPPUNIT_ASSERT(pmt_equal(pmt_deserialize(sb), PMT_F));

  CPPUNIT_ASSERT(pmt_equal(pmt_deserialize(sb), PMT_EOF));	// last item


  // FIXME add tests for real, complex, vector, uniform-vector, dict
  // FIXME add tests for malformed input too.

}

void
qa_pmt_prims::test_sets()
{
  pmt_t s1 = mp("s1");
  pmt_t s2 = mp("s2");
  pmt_t s3 = mp("s3");

  pmt_t l1 = pmt_list1(s1);
  pmt_t l2 = pmt_list2(s2,s3);
  pmt_t l3 = pmt_list3(s1,s2,s3);

  CPPUNIT_ASSERT(pmt_is_pair(pmt_memq(s1,l1)));
  CPPUNIT_ASSERT(pmt_is_false(pmt_memq(s3,l1)));

  CPPUNIT_ASSERT(pmt_subsetp(l1,l3));
  CPPUNIT_ASSERT(pmt_subsetp(l2,l3));
  CPPUNIT_ASSERT(!pmt_subsetp(l1,l2));
  CPPUNIT_ASSERT(!pmt_subsetp(l2,l1));
  CPPUNIT_ASSERT(!pmt_subsetp(l3,l2));
}

void
qa_pmt_prims::test_sugar()
{
  CPPUNIT_ASSERT(pmt_is_symbol(mp("my-symbol")));
  CPPUNIT_ASSERT_EQUAL((long) 10, pmt_to_long(mp(10)));
  CPPUNIT_ASSERT_EQUAL((double) 1e6, pmt_to_double(mp(1e6)));
  CPPUNIT_ASSERT_EQUAL(std::complex<double>(2, 3),
		       pmt_to_complex(mp(std::complex<double>(2, 3))));

  int buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  pmt_t blob = mp(buf, sizeof(buf));
  const void *data = pmt_blob_data(blob);
  size_t nbytes = pmt_blob_length(blob);
  CPPUNIT_ASSERT_EQUAL(sizeof(buf), nbytes);
  CPPUNIT_ASSERT(memcmp(buf, data, nbytes) == 0);
}

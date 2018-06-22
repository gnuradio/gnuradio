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

#include <pmt/api.h> //reason: suppress warnings
#include <gnuradio/messages/msg_passing.h>
#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>
#include <cstdio>
#include <cstring>
#include <sstream>

BOOST_AUTO_TEST_CASE(test_symbols) {
  BOOST_CHECK(!pmt::is_symbol(pmt::PMT_T));
  BOOST_CHECK(!pmt::is_symbol(pmt::PMT_F));
  BOOST_CHECK_THROW(pmt::symbol_to_string(pmt::PMT_F), pmt::wrong_type);

  pmt::pmt_t sym1 = pmt::mp("test");
  BOOST_CHECK(pmt::is_symbol(sym1));
  BOOST_CHECK_EQUAL(std::string("test"), pmt::symbol_to_string(sym1));
  BOOST_CHECK(pmt::is_true(sym1));
  BOOST_CHECK(!pmt::is_false(sym1));

  pmt::pmt_t sym2 = pmt::mp("foo");
  pmt::pmt_t sym3 = pmt::mp("test");
  BOOST_CHECK_EQUAL(sym1, sym3);
  BOOST_CHECK(sym1 != sym2);
  BOOST_CHECK(sym1 == sym3);

  static const int N = 2048;
  std::vector<pmt::pmt_t> v1(N);
  std::vector<pmt::pmt_t> v2(N);

  // generate a bunch of symbols
  for (int i = 0; i < N; i++){
    std::string buf = str(boost::format("test-%d") % i);
    v1[i] = pmt::mp(buf.c_str());
  }

  // confirm that they are all unique
  for (int i = 0; i < N; i++)
    for (int j = i + 1; j < N; j++)
      BOOST_CHECK(v1[i] != v1[j]);

  // generate the same symbols again
  for (int i = 0; i < N; i++){
    std::string buf = str(boost::format("test-%d") % i);
    v2[i] = pmt::mp(buf.c_str());
  }

  // confirm that we get the same ones back
  for (int i = 0; i < N; i++)
    BOOST_CHECK(v1[i] == v2[i]);
}

BOOST_AUTO_TEST_CASE(test_booleans) {
  pmt::pmt_t sym = pmt::mp("test");
  BOOST_CHECK(pmt::is_bool(pmt::PMT_T));
  BOOST_CHECK(pmt::is_bool(pmt::PMT_F));
  BOOST_CHECK(!pmt::is_bool(sym));
  BOOST_CHECK_EQUAL(pmt::from_bool(false), pmt::PMT_F);
  BOOST_CHECK_EQUAL(pmt::from_bool(true), pmt::PMT_T);
  BOOST_CHECK_EQUAL(false, pmt::to_bool(pmt::PMT_F));
  BOOST_CHECK_EQUAL(true, pmt::to_bool(pmt::PMT_T));
  BOOST_CHECK_THROW(pmt::to_bool(sym), pmt::wrong_type);
}

BOOST_AUTO_TEST_CASE(test_integers) {
  pmt::pmt_t p1 = pmt::from_long(1);
  pmt::pmt_t m1 = pmt::from_long(-1);
  BOOST_CHECK(!pmt::is_integer(pmt::PMT_T));
  BOOST_CHECK(pmt::is_integer(p1));
  BOOST_CHECK(pmt::is_integer(m1));
  BOOST_CHECK_THROW(pmt::to_long(pmt::PMT_T), pmt::wrong_type);
  BOOST_CHECK_EQUAL(-1L, pmt::to_long(m1));
  BOOST_CHECK_EQUAL(1L, pmt::to_long(p1));
}

BOOST_AUTO_TEST_CASE(test_uint64s) {
  pmt::pmt_t p1 = pmt::from_uint64((uint64_t)1);
  pmt::pmt_t m1 = pmt::from_uint64((uint64_t)8589934592ULL);
  BOOST_CHECK(!pmt::is_uint64(pmt::PMT_T));
  BOOST_CHECK(pmt::is_uint64(p1));
  BOOST_CHECK(pmt::is_uint64(m1));
  BOOST_CHECK_THROW(pmt::to_uint64(pmt::PMT_T), pmt::wrong_type);
  BOOST_CHECK_EQUAL((uint64_t)8589934592ULL, (uint64_t)pmt::to_uint64(m1));
  BOOST_CHECK_EQUAL((uint64_t)1ULL, (uint64_t)pmt::to_uint64(p1));
}

BOOST_AUTO_TEST_CASE(test_reals) {
  pmt::pmt_t p1 = pmt::from_double(1);
  pmt::pmt_t m1 = pmt::from_double(-1);
  BOOST_CHECK(!pmt::is_real(pmt::PMT_T));
  BOOST_CHECK(pmt::is_real(p1));
  BOOST_CHECK(pmt::is_real(m1));
  BOOST_CHECK_THROW(pmt::to_double(pmt::PMT_T), pmt::wrong_type);
  BOOST_CHECK_EQUAL(-1.0, pmt::to_double(m1));
  BOOST_CHECK_EQUAL(1.0, pmt::to_double(p1));
  BOOST_CHECK_EQUAL(1.0, pmt::to_double(pmt::from_long(1)));

  pmt::pmt_t p2 = pmt::from_float(1);
  pmt::pmt_t m2 = pmt::from_float(-1);
  BOOST_CHECK(pmt::is_real(p2));
  BOOST_CHECK(pmt::is_real(m2));
  BOOST_CHECK_THROW(pmt::to_float(pmt::PMT_T), pmt::wrong_type);
  BOOST_CHECK_EQUAL(float(-1.0), pmt::to_float(m2));
  BOOST_CHECK_EQUAL(float(1.0), pmt::to_float(p2));
  BOOST_CHECK_EQUAL(float(1.0), pmt::to_float(pmt::from_long(1)));
}

BOOST_AUTO_TEST_CASE(test_complexes) {
  pmt::pmt_t p1 = pmt::make_rectangular(2, -3);
  pmt::pmt_t m1 = pmt::make_rectangular(-3, 2);
  pmt::pmt_t p2 = pmt::from_complex(2, -3);
  pmt::pmt_t m2 = pmt::from_complex(-3, 2);
  pmt::pmt_t p3 = pmt::from_complex(std::complex<double>(2, -3));
  pmt::pmt_t m3 = pmt::from_complex(std::complex<double>(-3, 2));
  BOOST_CHECK(!pmt::is_complex(pmt::PMT_T));
  BOOST_CHECK(pmt::is_complex(p1));
  BOOST_CHECK(pmt::is_complex(m1));
  BOOST_CHECK(pmt::is_complex(p2));
  BOOST_CHECK(pmt::is_complex(m2));
  BOOST_CHECK(pmt::is_complex(p3));
  BOOST_CHECK(pmt::is_complex(m3));
  BOOST_CHECK_THROW(pmt::to_complex(pmt::PMT_T), pmt::wrong_type);
  BOOST_CHECK_EQUAL(std::complex<double>(2, -3), pmt::to_complex(p1));
  BOOST_CHECK_EQUAL(std::complex<double>(-3, 2), pmt::to_complex(m1));
  BOOST_CHECK_EQUAL(std::complex<double>(2, -3), pmt::to_complex(p2));
  BOOST_CHECK_EQUAL(std::complex<double>(-3, 2), pmt::to_complex(m2));
  BOOST_CHECK_EQUAL(std::complex<double>(2, -3), pmt::to_complex(p3));
  BOOST_CHECK_EQUAL(std::complex<double>(-3, 2), pmt::to_complex(m3));
  BOOST_CHECK_EQUAL(std::complex<double>(1.0, 0), pmt::to_complex(pmt::from_long(1)));
  BOOST_CHECK_EQUAL(std::complex<double>(1.0, 0), pmt::to_complex(pmt::from_double(1.0)));
}

BOOST_AUTO_TEST_CASE(test_pairs) {
  BOOST_CHECK(pmt::is_null(pmt::PMT_NIL));
  BOOST_CHECK(!pmt::is_pair(pmt::PMT_NIL));
  pmt::pmt_t s1 = pmt::mp("s1");
  pmt::pmt_t s2 = pmt::mp("s2");
  pmt::pmt_t s3 = pmt::mp("s3");


  BOOST_CHECK_EQUAL((size_t)0, pmt::length(pmt::PMT_NIL));
  BOOST_CHECK_THROW(pmt::length(s1), pmt::wrong_type);
  BOOST_CHECK_THROW(pmt::length(pmt::from_double(42)), pmt::wrong_type);

  pmt::pmt_t c1 = pmt::cons(s1, pmt::PMT_NIL);
  BOOST_CHECK(pmt::is_pair(c1));
  BOOST_CHECK(!pmt::is_pair(s1));
  BOOST_CHECK_EQUAL(s1, pmt::car(c1));
  BOOST_CHECK_EQUAL(pmt::PMT_NIL, pmt::cdr(c1));
  BOOST_CHECK_EQUAL((size_t) 1, pmt::length(c1));

  pmt::pmt_t c3 = pmt::cons(s3, pmt::PMT_NIL);
  pmt::pmt_t c2 = pmt::cons(s2, c3);
  pmt::set_cdr(c1, c2);
  BOOST_CHECK_EQUAL(c2, pmt::cdr(c1));
  pmt::set_car(c1, s3);
  BOOST_CHECK_EQUAL(s3, pmt::car(c1));
  BOOST_CHECK_EQUAL((size_t)1, pmt::length(c3));
  BOOST_CHECK_EQUAL((size_t)2, pmt::length(c2));

  BOOST_CHECK_THROW(pmt::cdr(pmt::PMT_NIL), pmt::wrong_type);
  BOOST_CHECK_THROW(pmt::car(pmt::PMT_NIL), pmt::wrong_type);
  BOOST_CHECK_THROW(pmt::set_car(s1, pmt::PMT_NIL), pmt::wrong_type);
  BOOST_CHECK_THROW(pmt::set_cdr(s1, pmt::PMT_NIL), pmt::wrong_type);
}

BOOST_AUTO_TEST_CASE(test_vectors) {
  static const size_t N = 3;
  pmt::pmt_t v1 = pmt::make_vector(N, pmt::PMT_NIL);
  BOOST_CHECK_EQUAL(N, pmt::length(v1));
  pmt::pmt_t s0 = pmt::mp("s0");
  pmt::pmt_t s1 = pmt::mp("s1");
  pmt::pmt_t s2 = pmt::mp("s2");

  pmt::vector_set(v1, 0, s0);
  pmt::vector_set(v1, 1, s1);
  pmt::vector_set(v1, 2, s2);

  BOOST_CHECK_EQUAL(s0, pmt::vector_ref(v1, 0));
  BOOST_CHECK_EQUAL(s1, pmt::vector_ref(v1, 1));
  BOOST_CHECK_EQUAL(s2, pmt::vector_ref(v1, 2));

  BOOST_CHECK_THROW(pmt::vector_ref(v1, N), pmt::out_of_range);
  BOOST_CHECK_THROW(pmt::vector_set(v1, N, pmt::PMT_NIL), pmt::out_of_range);

  pmt::vector_fill(v1, s0);
  for (size_t i = 0; i < N; i++)
    BOOST_CHECK_EQUAL(s0, pmt::vector_ref(v1, i));
}

static void
check_tuple(size_t len, const std::vector<pmt::pmt_t> &s, pmt::pmt_t t)
{
  BOOST_CHECK_EQUAL(true, pmt::is_tuple(t));
  BOOST_CHECK_EQUAL(len, pmt::length(t));

  for (size_t i = 0; i < len; i++)
    BOOST_CHECK_EQUAL(s[i], pmt::tuple_ref(t, i));

}

BOOST_AUTO_TEST_CASE(test_tuples) {
  pmt::pmt_t v = pmt::make_vector(10, pmt::PMT_NIL);
  std::vector<pmt::pmt_t> s(10);
  for (size_t i = 0; i < 10; i++){
    std::ostringstream os;
    os << "s" << i;
    s[i] = pmt::mp(os.str());
    pmt::vector_set(v, i, s[i]);
  }


  pmt::pmt_t t;

  t = pmt::make_tuple();
  check_tuple(0, s, t);

  t = pmt::make_tuple(s[0]);
  check_tuple(1, s, t);

  BOOST_CHECK(pmt::is_vector(v));
  BOOST_CHECK(!pmt::is_tuple(v));
  BOOST_CHECK(pmt::is_tuple(t));
  BOOST_CHECK(!pmt::is_vector(t));

  t = pmt::make_tuple(s[0], s[1]);
  check_tuple(2, s, t);

  t = pmt::make_tuple(s[0], s[1], s[2]);
  check_tuple(3, s, t);

  t = pmt::make_tuple(s[0], s[1], s[2], s[3]);
  check_tuple(4, s, t);

  t = pmt::make_tuple(s[0], s[1], s[2], s[3], s[4]);
  check_tuple(5, s, t);

  t = pmt::make_tuple(s[0], s[1], s[2], s[3], s[4], s[5]);
  check_tuple(6, s, t);

  t = pmt::make_tuple(s[0], s[1], s[2], s[3], s[4], s[5], s[6]);
  check_tuple(7, s, t);

  t = pmt::make_tuple(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7]);
  check_tuple(8, s, t);

  t = pmt::make_tuple(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8]);
  check_tuple(9, s, t);

  t = pmt::make_tuple(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8], s[9]);
  check_tuple(10, s, t);

  t = pmt::make_tuple(s[0], s[1], s[2]);
  BOOST_CHECK_THROW(pmt::tuple_ref(t, 3), pmt::out_of_range);
  BOOST_CHECK_THROW(pmt::vector_ref(t, 0), pmt::wrong_type);
  BOOST_CHECK_THROW(pmt::tuple_ref(v, 0), pmt::wrong_type);

  t = pmt::make_tuple(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8], s[9]);
  pmt::pmt_t t2 = pmt::to_tuple(v);
  BOOST_CHECK_EQUAL(size_t(10), pmt::length(v));
  BOOST_CHECK(pmt::equal(t, t2));
  //std::cout << v << std::endl;
  //std::cout << t2 << std::endl;


  t = pmt::make_tuple(s[0], s[1], s[2]);
  pmt::pmt_t list0 = pmt::list3(s[0], s[1], s[2]);
  BOOST_CHECK_EQUAL(size_t(3), pmt::length(list0));
  t2 = pmt::to_tuple(list0);
  BOOST_CHECK_EQUAL(size_t(3), pmt::length(t2));
  BOOST_CHECK(pmt::equal(t, t2));
}

BOOST_AUTO_TEST_CASE(test_equivalence) {
  pmt::pmt_t s0 = pmt::mp("s0");
  pmt::pmt_t s1 = pmt::mp("s1");
  pmt::pmt_t s2 = pmt::mp("s2");
  pmt::pmt_t list0 = pmt::cons(s0, pmt::cons(s1, pmt::cons(s2, pmt::PMT_NIL)));
  pmt::pmt_t list1 = pmt::cons(s0, pmt::cons(s1, pmt::cons(s2, pmt::PMT_NIL)));
  pmt::pmt_t i0 = pmt::from_long(42);
  pmt::pmt_t i1 = pmt::from_long(42);
  pmt::pmt_t r0 = pmt::from_double(42);
  pmt::pmt_t r1 = pmt::from_double(42);
  pmt::pmt_t r2 = pmt::from_double(43);

  BOOST_CHECK(pmt::eq(s0, s0));
  BOOST_CHECK(!pmt::eq(s0, s1));
  BOOST_CHECK(pmt::eqv(s0, s0));
  BOOST_CHECK(!pmt::eqv(s0, s1));

  BOOST_CHECK(pmt::eqv(i0, i1));
  BOOST_CHECK(pmt::eqv(r0, r1));
  BOOST_CHECK(!pmt::eqv(r0, r2));
  BOOST_CHECK(!pmt::eqv(i0, r0));

  BOOST_CHECK(!pmt::eq(list0, list1));
  BOOST_CHECK(!pmt::eqv(list0, list1));
  BOOST_CHECK(pmt::equal(list0, list1));

  pmt::pmt_t v0 = pmt::make_vector(3, s0);
  pmt::pmt_t v1 = pmt::make_vector(3, s0);
  pmt::pmt_t v2 = pmt::make_vector(4, s0);
  BOOST_CHECK(!pmt::eqv(v0, v1));
  BOOST_CHECK(pmt::equal(v0, v1));
  BOOST_CHECK(!pmt::equal(v0, v2));

  pmt::vector_set(v0, 0, list0);
  pmt::vector_set(v0, 1, list0);
  pmt::vector_set(v1, 0, list1);
  pmt::vector_set(v1, 1, list1);
  BOOST_CHECK(pmt::equal(v0, v1));
}

BOOST_AUTO_TEST_CASE(test_misc) {
  pmt::pmt_t k0 = pmt::mp("k0");
  pmt::pmt_t k1 = pmt::mp("k1");
  pmt::pmt_t k2 = pmt::mp("k2");
  pmt::pmt_t k3 = pmt::mp("k3");
  pmt::pmt_t v0 = pmt::mp("v0");
  pmt::pmt_t v1 = pmt::mp("v1");
  pmt::pmt_t v2 = pmt::mp("v2");
  pmt::pmt_t p0 = pmt::cons(k0, v0);
  pmt::pmt_t p1 = pmt::cons(k1, v1);
  pmt::pmt_t p2 = pmt::cons(k2, v2);

  pmt::pmt_t alist = pmt::cons(p0, pmt::cons(p1, pmt::cons(p2, pmt::PMT_NIL)));
  BOOST_CHECK(pmt::eq(p1, pmt::assv(k1, alist)));
  BOOST_CHECK(pmt::eq(pmt::PMT_F, pmt::assv(k3, alist)));

  pmt::pmt_t keys = pmt::cons(k0, pmt::cons(k1, pmt::cons(k2, pmt::PMT_NIL)));
  pmt::pmt_t vals = pmt::cons(v0, pmt::cons(v1, pmt::cons(v2, pmt::PMT_NIL)));
  BOOST_CHECK(pmt::equal(keys, pmt::map(pmt::car, alist)));
  BOOST_CHECK(pmt::equal(vals, pmt::map(pmt::cdr, alist)));
}

BOOST_AUTO_TEST_CASE(test_dict) {
  pmt::pmt_t dict = pmt::make_dict();
  BOOST_CHECK(pmt::is_dict(dict));

  pmt::pmt_t k0 = pmt::mp("k0");
  pmt::pmt_t k1 = pmt::mp("k1");
  pmt::pmt_t k2 = pmt::mp("k2");
  pmt::pmt_t k3 = pmt::mp("k3");
  pmt::pmt_t v0 = pmt::mp("v0");
  pmt::pmt_t v1 = pmt::mp("v1");
  pmt::pmt_t v2 = pmt::mp("v2");
  pmt::pmt_t v3 = pmt::mp("v3");
  pmt::pmt_t not_found = pmt::cons(pmt::PMT_NIL, pmt::PMT_NIL);

  BOOST_CHECK(!pmt::dict_has_key(dict, k0));
  dict = pmt::dict_add(dict, k0, v0);
  BOOST_CHECK(pmt::dict_has_key(dict, k0));
  BOOST_CHECK(pmt::eqv(pmt::dict_ref(dict, k0, not_found), v0));
  BOOST_CHECK(pmt::eqv(pmt::dict_ref(dict, k1, not_found), not_found));
  dict = pmt::dict_add(dict, k1, v1);
  dict = pmt::dict_add(dict, k2, v2);
  BOOST_CHECK(pmt::eqv(pmt::dict_ref(dict, k1, not_found), v1));
  dict = pmt::dict_add(dict, k1, v3);
  BOOST_CHECK(pmt::eqv(pmt::dict_ref(dict, k1, not_found), v3));

  pmt::pmt_t keys = pmt::list3(k1, k2, k0);
  pmt::pmt_t vals = pmt::list3(v3, v2, v0);
  //std::cout << "pmt::dict_keys:   " << pmt::dict_keys(dict) << std::endl;
  //std::cout << "pmt::dict_values: " << pmt::dict_values(dict) << std::endl;
  BOOST_CHECK(pmt::equal(keys, pmt::dict_keys(dict)));
  BOOST_CHECK(pmt::equal(vals, pmt::dict_values(dict)));
}

BOOST_AUTO_TEST_CASE(test_io) {
  pmt::pmt_t k0 = pmt::mp("k0");
  pmt::pmt_t k1 = pmt::mp("k1");
  pmt::pmt_t k2 = pmt::mp("k2");
  pmt::pmt_t k3 = pmt::mp("k3");

  BOOST_CHECK_EQUAL(std::string("k0"), pmt::write_string(k0));
}

BOOST_AUTO_TEST_CASE(test_lists) {
  pmt::pmt_t s0 = pmt::mp("s0");
  pmt::pmt_t s1 = pmt::mp("s1");
  pmt::pmt_t s2 = pmt::mp("s2");
  pmt::pmt_t s3 = pmt::mp("s3");

  pmt::pmt_t l1 = pmt::list4(s0, s1, s2, s3);
  pmt::pmt_t l2 = pmt::list3(s0, s1, s2);
  pmt::pmt_t l3 = pmt::list_add(l2, s3);
  BOOST_CHECK(pmt::equal(l1, l3));
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

BOOST_AUTO_TEST_CASE(test_any) {
  boost::any a0;
  boost::any a1;
  boost::any a2;

  a0 = std::string("Hello!");
  a1 = 42;
  a2 = foo(3.250, 21);

  pmt::pmt_t p0 = pmt::make_any(a0);
  pmt::pmt_t p1 = pmt::make_any(a1);
  pmt::pmt_t p2 = pmt::make_any(a2);

  BOOST_CHECK_EQUAL(std::string("Hello!"),
		       boost::any_cast<std::string>(pmt::any_ref(p0)));

  BOOST_CHECK_EQUAL(42,
		       boost::any_cast<int>(pmt::any_ref(p1)));

  BOOST_CHECK_EQUAL(foo(3.250, 21),
		       boost::any_cast<foo>(pmt::any_ref(p2)));
}

// ------------------------------------------------------------------------

class qa_pmt_msg_accepter_nop : public gr::messages::msg_accepter
{
public:
  qa_pmt_msg_accepter_nop(){}
  ~qa_pmt_msg_accepter_nop();
  void post(pmt::pmt_t,pmt::pmt_t){}
};

qa_pmt_msg_accepter_nop::~qa_pmt_msg_accepter_nop(){}

BOOST_AUTO_TEST_CASE(test_msg_accepter) {
  pmt::pmt_t sym = pmt::mp("my-symbol");

  boost::any a0;
  a0 = std::string("Hello!");
  pmt::pmt_t p0 = pmt::make_any(a0);

  gr::messages::msg_accepter_sptr ma0 =                                 \
    gr::messages::msg_accepter_sptr(new qa_pmt_msg_accepter_nop());
  pmt::pmt_t p1 = pmt::make_msg_accepter(ma0);

  BOOST_CHECK_EQUAL(ma0.get(), pmt::msg_accepter_ref(p1).get());

  BOOST_CHECK_THROW(pmt::msg_accepter_ref(sym), pmt::wrong_type);
  BOOST_CHECK_THROW(pmt::msg_accepter_ref(p0),  pmt::wrong_type);

  // just confirm interfaces on send are OK
  pmt::pmt_t port(pmt::intern("port"));
  gr::messages::send(ma0.get(), port, sym);
  gr::messages::send(ma0, port, sym);
  gr::messages::send(p1, port, sym);

}

// ------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(test_serialize) {
  std::stringbuf sb;		// fake channel
  pmt::pmt_t a = pmt::mp("a");
  pmt::pmt_t b = pmt::mp("b");
  pmt::pmt_t c = pmt::mp("c");

  sb.str("");			// reset channel to empty

  // write stuff to channel

  pmt::serialize(pmt::PMT_NIL, sb);
  pmt::serialize(pmt::mp("foobarvia"), sb);
  pmt::serialize(pmt::from_long(123456789), sb);
  pmt::serialize(pmt::from_long(-123456789), sb);
  pmt::serialize(pmt::cons(pmt::PMT_NIL, pmt::PMT_NIL), sb);
  pmt::serialize(pmt::cons(a, b), sb);
  pmt::serialize(pmt::list1(a), sb);
  pmt::serialize(pmt::list2(a, b), sb);
  pmt::serialize(pmt::list3(a, b, c), sb);
  pmt::serialize(pmt::list3(a, pmt::list3(c, b, a), c), sb);
  pmt::serialize(pmt::PMT_T, sb);
  pmt::serialize(pmt::PMT_F, sb);

  // read it back

  BOOST_CHECK(pmt::equal(pmt::deserialize(sb), pmt::PMT_NIL));
  BOOST_CHECK(pmt::equal(pmt::deserialize(sb), pmt::mp("foobarvia")));
  BOOST_CHECK(pmt::equal(pmt::deserialize(sb), pmt::from_long(123456789)));
  BOOST_CHECK(pmt::equal(pmt::deserialize(sb), pmt::from_long(-123456789)));
  BOOST_CHECK(pmt::equal(pmt::deserialize(sb), pmt::cons(pmt::PMT_NIL, pmt::PMT_NIL)));
  BOOST_CHECK(pmt::equal(pmt::deserialize(sb), pmt::cons(a, b)));
  BOOST_CHECK(pmt::equal(pmt::deserialize(sb), pmt::list1(a)));
  BOOST_CHECK(pmt::equal(pmt::deserialize(sb), pmt::list2(a, b)));
  BOOST_CHECK(pmt::equal(pmt::deserialize(sb), pmt::list3(a, b, c)));
  BOOST_CHECK(pmt::equal(pmt::deserialize(sb), pmt::list3(a, pmt::list3(c, b, a), c)));
  BOOST_CHECK(pmt::equal(pmt::deserialize(sb), pmt::PMT_T));
  BOOST_CHECK(pmt::equal(pmt::deserialize(sb), pmt::PMT_F));

  BOOST_CHECK(pmt::equal(pmt::deserialize(sb), pmt::PMT_EOF));	// last item


  // FIXME add tests for real, complex, vector, uniform-vector, dict
  // FIXME add tests for malformed input too.

}

BOOST_AUTO_TEST_CASE(test_sets) {
  pmt::pmt_t s1 = pmt::mp("s1");
  pmt::pmt_t s2 = pmt::mp("s2");
  pmt::pmt_t s3 = pmt::mp("s3");

  pmt::pmt_t l1 = pmt::list1(s1);
  pmt::pmt_t l2 = pmt::list2(s2,s3);
  pmt::pmt_t l3 = pmt::list3(s1,s2,s3);

  BOOST_CHECK(pmt::is_pair(pmt::memq(s1,l1)));
  BOOST_CHECK(pmt::is_false(pmt::memq(s3,l1)));

  BOOST_CHECK(pmt::subsetp(l1,l3));
  BOOST_CHECK(pmt::subsetp(l2,l3));
  BOOST_CHECK(!pmt::subsetp(l1,l2));
  BOOST_CHECK(!pmt::subsetp(l2,l1));
  BOOST_CHECK(!pmt::subsetp(l3,l2));
}

BOOST_AUTO_TEST_CASE(test_sugar) {
  BOOST_CHECK(pmt::is_symbol(pmt::mp("my-symbol")));
  BOOST_CHECK_EQUAL((long) 10, pmt::to_long(pmt::mp(10)));
  BOOST_CHECK_EQUAL((double) 1e6, pmt::to_double(pmt::mp(1e6)));
  BOOST_CHECK_EQUAL(std::complex<double>(2, 3),
		       pmt::to_complex(pmt::mp(std::complex<double>(2, 3))));

  int buf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  pmt::pmt_t blob = pmt::mp(buf, sizeof(buf));
  const void *data = pmt::blob_data(blob);
  size_t nbytes = pmt::blob_length(blob);
  BOOST_CHECK_EQUAL(sizeof(buf), nbytes);
  BOOST_CHECK(memcmp(buf, data, nbytes) == 0);
}


BOOST_AUTO_TEST_CASE(test_@TAG@vector) {
  static const size_t N = 3;
  pmt::pmt_t v1 = pmt::make_@TAG@vector(N, 0);
  BOOST_CHECK_EQUAL(N, pmt::length(v1));
  @TYPE@ s0 = @TYPE@(10);
  @TYPE@ s1 = @TYPE@(20);
  @TYPE@ s2 = @TYPE@(30);

  pmt::@TAG@vector_set(v1, 0, s0);
  pmt::@TAG@vector_set(v1, 1, s1);
  pmt::@TAG@vector_set(v1, 2, s2);

  BOOST_CHECK_EQUAL(s0, pmt::@TAG@vector_ref(v1, 0));
  BOOST_CHECK_EQUAL(s1, pmt::@TAG@vector_ref(v1, 1));
  BOOST_CHECK_EQUAL(s2, pmt::@TAG@vector_ref(v1, 2));

  BOOST_CHECK_THROW(pmt::@TAG@vector_ref(v1, N), pmt::out_of_range);
  BOOST_CHECK_THROW(pmt::@TAG@vector_set(v1, N, @TYPE@(0)), pmt::out_of_range);

  size_t	len;
  const @TYPE@ *rd = pmt::@TAG@vector_elements(v1, len);
  BOOST_CHECK_EQUAL(len, N);
  BOOST_CHECK_EQUAL(s0, rd[0]);
  BOOST_CHECK_EQUAL(s1, rd[1]);
  BOOST_CHECK_EQUAL(s2, rd[2]);

  @TYPE@ *wr = pmt::@TAG@vector_writable_elements(v1, len);
  BOOST_CHECK_EQUAL(len, N);
  wr[0] = @TYPE@(0);
  BOOST_CHECK_EQUAL(@TYPE@(0), wr[0]);
  BOOST_CHECK_EQUAL(s1, wr[1]);
  BOOST_CHECK_EQUAL(s2, wr[2]);
}

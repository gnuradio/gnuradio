#ifndef INCLUDED_QA_16SC_DEINTERLEAVE_REAL_8S_ALIGNED16_H
#define INCLUDED_QA_16SC_DEINTERLEAVE_REAL_8S_ALIGNED16_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

class qa_16sc_deinterleave_real_8s_aligned16 : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE (qa_16sc_deinterleave_real_8s_aligned16);
  CPPUNIT_TEST (t1);
  CPPUNIT_TEST_SUITE_END ();

 private:
  void t1 ();
};


#endif /* INCLUDED_QA_16SC_DEINTERLEAVE_REAL_8S_ALIGNED16_H */

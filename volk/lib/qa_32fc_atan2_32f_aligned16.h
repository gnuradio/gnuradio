#ifndef INCLUDED_QA_32FC_ATAN2_32F_ALIGNED16_H
#define INCLUDED_QA_32FC_ATAN2_32F_ALIGNED16_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

class qa_32fc_atan2_32f_aligned16 : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE (qa_32fc_atan2_32f_aligned16);
  CPPUNIT_TEST (t1);
  CPPUNIT_TEST_SUITE_END ();

 private:
  void t1 ();
};


#endif /* INCLUDED_QA_32FC_ATAN2_32F_ALIGNED16_H */

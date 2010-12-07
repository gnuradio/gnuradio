#ifndef INCLUDED_QA_32S_CONVERT_32F_UNALIGNED16_H
#define INCLUDED_QA_32S_CONVERT_32F_UNALIGNED16_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

class qa_32s_convert_32f_unaligned16 : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE (qa_32s_convert_32f_unaligned16);
  CPPUNIT_TEST (t1);
  CPPUNIT_TEST_SUITE_END ();

 private:
  void t1 ();
};


#endif /* INCLUDED_QA_32S_CONVERT_32F_UNALIGNED16_H */

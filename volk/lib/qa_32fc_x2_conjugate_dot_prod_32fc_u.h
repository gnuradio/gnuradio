#ifndef INCLUDED_QA_32FC_X2_CONJUGATE_DOT_PROD_32FC_U_H
#define INCLUDED_QA_32FC_X2_CONJUGATE_DOT_PROD_32FC_U_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

class qa_32fc_x2_conjugate_dot_prod_32fc_u : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE (qa_32fc_x2_conjugate_dot_prod_32fc_u);
  CPPUNIT_TEST (t1);
  CPPUNIT_TEST_SUITE_END ();

 private:
  void t1 ();
};


#endif /* INCLUDED_QA_32FC_X2_CONJUGATE_DOT_PROD_32FC_U_H */

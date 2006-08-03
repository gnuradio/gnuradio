/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef _QA_GR_VMCIRCBUF_H_
#define _QA_GR_VMCIRCBUF_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

class qa_gr_vmcircbuf : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE (qa_gr_vmcircbuf);
  CPPUNIT_TEST (test_all);
  CPPUNIT_TEST_SUITE_END ();

 private:
  void test_all ();
};


#endif /* _QA_GR_VMCIRCBUF_H_ */

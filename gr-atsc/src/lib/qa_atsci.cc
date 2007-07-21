/*
 * Copyright 2002 Free Software Foundation, Inc.
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

/*
 * This class gathers together all the test cases for the atsc
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include <qa_atsci.h>
#include <qa_atsci_randomizer.h>
#include <qa_atsci_reed_solomon.h>
#include <qa_interleaver_fifo.h>
#include <qa_convolutional_interleaver.h>
#include <qa_atsci_data_interleaver.h>
#include <qa_atsci_basic_trellis_encoder.h>
#include <qa_atsci_sliding_correlator.h>
#include <qa_atsci_fake_single_viterbi.h>
#include <qa_atsci_single_viterbi.h>
#include <qa_atsci_trellis_encoder.h>
#include <qa_atsci_viterbi_decoder.h>
#include <qa_atsci_fs_correlator.h>
#include <qa_atsci_equalizer_nop.h>

CppUnit::TestSuite *
qa_atsc::suite ()
{
  CppUnit::TestSuite	*s = new CppUnit::TestSuite ("atsc");

  s->addTest (qa_atsci_randomizer::suite ());
  s->addTest (qa_atsci_reed_solomon::suite ());
  s->addTest (qa_interleaver_fifo::suite ());
  s->addTest (qa_convolutional_interleaver::suite ());
  s->addTest (qa_atsci_data_interleaver::suite ());
  s->addTest (qa_atsci_basic_trellis_encoder::suite ());
  s->addTest (qa_atsci_sliding_correlator::suite ());
  s->addTest (qa_atsci_fake_single_viterbi::suite ());
  s->addTest (qa_atsci_single_viterbi::suite ());
  s->addTest (qa_atsci_trellis_encoder::suite ());
  s->addTest (qa_atsci_viterbi_decoder::suite ());
  s->addTest (qa_atsci_fs_correlator::suite ());
  s->addTest (qa_atsci_equalizer_nop::suite ());
  
  return s;
}

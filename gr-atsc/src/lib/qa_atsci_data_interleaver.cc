/* -*- c++ -*- */
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

#include <cppunit/TestAssert.h>
#include <qa_atsci_data_interleaver.h>
#include <string.h>


/*!
 * write an easy to identify pattern into the packet
 */
void
qa_atsci_data_interleaver::init_test_packet (int counter,
					    atsc_mpeg_packet_rs_encoded &out)
{
  out.data[0] = 0xf0;
  out.data[1] = 0xff;
  out.data[2] = (counter >> 8) & 0xff;
  out.data[3] = counter & 0xff;

  for (int i = 4; i < 205; i++)
    out.data[i] = i;

  out.data[205] = 0xa0;
  out.data[206] = 0xaf;
}

void
qa_atsci_data_interleaver::print_packet (FILE *fp,
					int frame_number,
					int field_number,
					int segment_number,
					const atsc_mpeg_packet_rs_encoded &in)
{
  fprintf (fp, "%04d:%d:%03d  ", frame_number, field_number, segment_number);

  const unsigned char *p = &in.data[0];
  for (int i = 0; i < 8; i++)
    fprintf (fp, "%02X", p[i]);

  fprintf (fp, "  ");
  p = &in.data[8];
  for (int i = 0; i < 8; i++)
    fprintf (fp, "%02X", p[i]);

  
  fprintf (fp, "  ...  ");
  p = &in.data[191];
  for (int i = 0; i < 8; i++)
    fprintf (fp, "%02X", p[i]);

  fprintf (fp, "  ");
  p = &in.data[199];
  for (int i = 0; i < 8; i++)
    fprintf (fp, "%02X", p[i]);

  fprintf (fp, "\n");
}

void
qa_atsci_data_interleaver::chk_assert (const atsc_mpeg_packet_rs_encoded &expected,
				      const atsc_mpeg_packet_rs_encoded &actual)
{
  if (expected == actual)
    return;

  FILE *fp = stderr;

  fprintf (fp, "Expected: ");
  print_packet (fp, 0, 0, 0, expected);

  fprintf (fp, "Actual:   ");
  print_packet (fp, 0, 0, 0, actual);

  CPPUNIT_ASSERT (expected == actual);
}

void
qa_atsci_data_interleaver::t0 ()
{
  int	counter = 0;
  atsc_mpeg_packet_rs_encoded	in, enc, out;
  atsc_mpeg_packet_rs_encoded	zero;

  memset (&zero, 0, sizeof (zero));
  
  for (int frame = 0; frame < 4; frame++){
    for (int field = 0; field < 2; field++){
      for (int segment = 0; segment < 312; segment++, counter++){

	// build test packet
	init_test_packet (counter, in);
	in.pli.set_regular_seg (field == 1, segment);

	// interleave it
	outbound.interleave (enc, in);

	// deinterleave it
	inbound.deinterleave (out, enc);

	if (counter < 52)
	  // CPPUNIT_ASSERT (zero == out);
	  chk_assert (zero, out);

	else if (counter >= 52){
	  atsc_mpeg_packet_rs_encoded expected;
	  init_test_packet (counter - 52, expected);
	  // CPPUNIT_ASSERT (expected == out);
	  chk_assert (expected, out);
	}
      }
    }
  }
}

//
// Note, this assumes that the interleaver and deinterleaver
// have the state left by t0.
//
// This test pushes crap into the interleaver, and then confirms that
// the deinterleaver recovers.  This would be part of what you'd see
// on a channel change.
//

void
qa_atsci_data_interleaver::t1 ()
{
  int	counter = 0;
  atsc_mpeg_packet_rs_encoded	in, enc, out;
  atsc_mpeg_packet_rs_encoded	zero;

  memset (&zero, 0, sizeof (zero));
  
  static const int NCRAP = 13;
  
  // push NCRAP segments of crap into the interleaver,
  // but don't change the deinterleaver state

  for (int i = 0; i < NCRAP; i++){
    init_test_packet (i + 2758, in);
    in.pli.set_regular_seg (false, i + 5);

    outbound.interleave (enc, in);
  }

  // now run the normal test.
  // we should get good segments after NCRAP + 52
  
  int starting_counter = 3141;
  counter = starting_counter;

  for (int frame = 0; frame < 4; frame++){
    for (int field = 0; field < 2; field++){
      for (int segment = 0; segment < 312; segment++, counter++){

	// build test packet
	init_test_packet (counter, in);
	in.pli.set_regular_seg (field == 1, segment);

	// interleave it
	outbound.interleave (enc, in);

	// deinterleave it
	inbound.deinterleave (out, enc);

	if (counter < starting_counter + 52 + NCRAP){
	  // don't care...
	}
	else if (counter >= starting_counter + 52 + NCRAP){
	  atsc_mpeg_packet_rs_encoded expected;
	  init_test_packet (counter - 52, expected);
	  // CPPUNIT_ASSERT (expected == out);
	  chk_assert (expected, out);
	}
      }
    }
  }
}

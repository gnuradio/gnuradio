/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
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
 * GNU Radio C++ example creating dial tone
 * ("the simplest thing that could possibly work")
 *
 * Send a tone each to the left and right channels of stereo audio
 * output and let the user's brain sum them.
 *
 * GNU Radio makes extensive use of Boost shared pointers.  Signal processing
 * blocks are typically created by calling a "make" factory function, which
 * returns an instance of the block as a typedef'd shared pointer that can
 * be used in any way a regular pointer can.  Shared pointers created this way
 * keep track of their memory and free it at the right time, so the user
 * doesn't need to worry about it (really).
 *
 */

// Include header files for each block used in flowgraph
#include <gnuradio/top_block.h>
#include <gnuradio/analog/sig_source_f.h>
#include <gnuradio/audio/sink.h>

using namespace gr;

int main(int argc, char **argv)
{
  int rate = 48000;		// Audio card sample rate
  float ampl = 0.1;		// Don't exceed 0.5 or clipping will occur

  // Construct a top block that will contain flowgraph blocks.  Alternatively,
  // one may create a derived class from top_block and hold instantiated blocks
  // as member data for later manipulation.
  top_block_sptr tb = make_top_block("dial_tone");

  // Construct a real-valued signal source for each tone, at given sample rate
  analog::sig_source_f::sptr src0 = analog::sig_source_f::make(rate, analog::GR_SIN_WAVE, 350, ampl);
  analog::sig_source_f::sptr src1 = analog::sig_source_f::make(rate, analog::GR_SIN_WAVE, 440, ampl);

  // Construct an audio sink to accept audio tones
  audio::sink::sptr sink = audio::sink::make(rate);

  // Connect output #0 of src0 to input #0 of sink (left channel)
  tb->connect(src0, 0, sink, 0);

  // Connect output #0 of src1 to input #1 of sink (right channel)
  tb->connect(src1, 0, sink, 1);

  // Tell GNU Radio runtime to start flowgraph threads; the foreground thread
  // will block until either flowgraph exits (this example doesn't) or the
  // application receives SIGINT (e.g., user hits CTRL-C).
  //
  // Real applications may use tb->start() which returns, allowing the foreground
  // thread to proceed, then later use tb->stop(), followed by tb->wait(), to cleanup
  // GNU Radio before exiting.
  tb->run();

  // Exit normally.
  return 0;
}

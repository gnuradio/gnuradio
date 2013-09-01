/*
 * Copyright 2013 Free Software Foundation, Inc.
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
 * This is a basic ATSC reciver program, built as an example on 
 * how to use libgnuradio-atsc to demodulate ATSC signals. It
 * is based on atsc_rx.py, refer there for more information.
 */

// Include header files for each block used in flowgraph
#include <iostream>
#include <gnuradio/top_block.h>
#include <gnuradio/blocks/file_source.h>
#include <gnuradio/blocks/interleaved_short_to_complex.h>
#include <gnuradio/atsc/consts.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/fir_filter_ccf.h>
#include <gnuradio/filter/interp_fir_filter_ccf.h>
#include <gnuradio/blocks/complex_to_float.h>
#include <gnuradio/atsc/fpll.h>
#include <gnuradio/filter/fir_filter_fff.h>
#include <gnuradio/filter/freq_xlating_fir_filter_ccf.h>
#include <gnuradio/analog/agc_ff.h>
#include <gnuradio/filter/single_pole_iir_filter_ff.h>
#include <gnuradio/blocks/sub_ff.h>
#include <gnuradio/atsc/bit_timing_loop.h>
#include <gnuradio/atsc/fs_checker.h>
#include <gnuradio/atsc/equalizer.h>
#include <gnuradio/atsc/viterbi_decoder.h>
#include <gnuradio/atsc/deinterleaver.h>
#include <gnuradio/atsc/rs_decoder.h>
#include <gnuradio/atsc/derandomizer.h>
#include <gnuradio/atsc/depad.h>
#include <gnuradio/blocks/file_sink.h>

#define INPUT_RATE (6.4e6)
#define INTERPOLATION (3)
#define INTERPOLATED_RATE (INPUT_RATE*INTERPOLATION)
#define INTERMEDIARY_FREQUENCY (5.75e6)

using namespace gr;

int main(int argc, char **argv)
{
	if( argc != 3 )
	{
		std::cout << "Usage: " << argv[0] << " input_file output_file.ts" << std::endl;
		return 1;
	}

	top_block_sptr tb = make_top_block("atsc_demod");

	// Read from input file
	blocks::file_source::sptr srcf = blocks::file_source::make(sizeof(short), argv[1]);

	// Convert interleaved shorts (I,Q,I,Q) to complex
	blocks::interleaved_short_to_complex::sptr is2c = blocks::interleaved_short_to_complex::make();

	// 1/2 as wide because we're designing lp filter
	float symbol_rate = ATSC_SYMBOL_RATE/2.0;
	int NTAPS = 279;
	std::vector<float> tt = filter::firdes::root_raised_cosine(1.0, INPUT_RATE, symbol_rate, 0.1152, NTAPS);
	filter::fir_filter_ccf::sptr rrc = filter::fir_filter_ccf::make(1, tt);

	// Interpolate Filter our 6MHz wide signal centered at 0
	std::vector<float> ilp_coeffs = filter::firdes::low_pass(3, INTERPOLATED_RATE, 3.2e6, 0.5e6, filter::firdes::WIN_HAMMING);
	filter::interp_fir_filter_ccf::sptr ilp = filter::interp_fir_filter_ccf::make(3, ilp_coeffs);

	// Move the center frequency to 5.75MHz ( this wont be needed soon )
	std::vector<float> duc_coeffs = filter::firdes::low_pass(1, INTERPOLATED_RATE, 9e6, 1e6, filter::firdes::WIN_HAMMING);
    	filter::freq_xlating_fir_filter_ccf::sptr duc = filter::freq_xlating_fir_filter_ccf::make(1, duc_coeffs, -INTERMEDIARY_FREQUENCY, INTERPOLATED_RATE);

	// fpll input is float
	blocks::complex_to_float::sptr c2f = blocks::complex_to_float::make();

	// Phase locked loop
	atsc_fpll_sptr fpll = atsc_make_fpll(INTERPOLATED_RATE);

	// Clean fpll output
	std::vector<float> lp_coeffs2 = filter::firdes::low_pass(1.0, INTERPOLATED_RATE, INTERMEDIARY_FREQUENCY, 800e3, filter::firdes::WIN_HAMMING);
	filter::fir_filter_fff::sptr lp_filter = filter::fir_filter_fff::make(1, lp_coeffs2);

	// Automatic gain control
	analog::agc_ff::sptr agc = analog::agc_ff::make(1e-6, 4);

	// Remove pilot ( at DC now )
	filter::single_pole_iir_filter_ff::sptr iir = filter::single_pole_iir_filter_ff::make(1e-5);
	blocks::sub_ff::sptr remove_dc = blocks::sub_ff::make();

	// Bit Timing Loop, Field Sync Checker and Equalizer
	atsc_bit_timing_loop_sptr btl = atsc_make_bit_timing_loop(INTERPOLATED_RATE);
	atsc_fs_checker_sptr fsc = atsc_make_fs_checker();
	atsc_equalizer_sptr eq = atsc_make_equalizer();

	// Viterbi
	atsc_viterbi_decoder_sptr viterbi = atsc_make_viterbi_decoder();
        atsc_deinterleaver_sptr deinter = atsc_make_deinterleaver();
        atsc_rs_decoder_sptr rs_dec = atsc_make_rs_decoder();
        atsc_derandomizer_sptr derand = atsc_make_derandomizer();
	atsc_depad_sptr depad = atsc_make_depad();

	// Write to output file
	blocks::file_sink::sptr outf = blocks::file_sink::make(sizeof(char),argv[2]);

	// Connect it all together
	tb->connect(srcf, 0, is2c, 0);
	tb->connect(is2c, 0, rrc, 0);
	tb->connect(rrc, 0, ilp, 0);
	tb->connect(ilp, 0, duc, 0);
	tb->connect(duc, 0, c2f, 0);
	tb->connect(c2f, 0, fpll, 0);
	tb->connect(fpll, 0, lp_filter, 0);
	tb->connect(lp_filter, 0, agc, 0);
	tb->connect(agc, 0, remove_dc, 0);
	tb->connect(agc, 0, iir, 0);
	tb->connect(iir, 0, remove_dc, 1);
	tb->connect(remove_dc, 0, btl, 0);
	tb->connect(btl, 0, fsc, 0);
	tb->connect(fsc, 0, eq, 0);
	tb->connect(eq, 0, viterbi, 0);
	tb->connect(viterbi, 0, deinter, 0);
	tb->connect(deinter, 0, rs_dec, 0);
	tb->connect(rs_dec, 0, derand, 0);
	tb->connect(derand, 0, depad, 0);
	tb->connect(depad, 0, outf, 0);

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


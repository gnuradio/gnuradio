/* -*- c++ -*- */
/*
 * Copyright 2006,2010 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/atsc/viterbi_decoder.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/atsc/consts.h>
#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <cmath>
#include "atsci_viterbi_mux.cc"
#include <string.h>

using std::cerr;
using std::endl;

/* How many separate Trellis encoders / Viterbi decoders run in parallel */
static const int	NCODERS = 12;

static const float	DSEG_SYNC_SYM1 =  5;
static const float	DSEG_SYNC_SYM2 = -5;
static const float	DSEG_SYNC_SYM3 = -5;
static const float	DSEG_SYNC_SYM4 =  5;

atsc_viterbi_decoder_sptr
atsc_make_viterbi_decoder()
{
	return gnuradio::get_initial_sptr(new atsc_viterbi_decoder());
}

atsc_viterbi_decoder::atsc_viterbi_decoder()
	: gr::sync_block("atsc_viterbi_decoder",
			gr::io_signature::make(1, 1, sizeof(atsc_soft_data_segment)),
			gr::io_signature::make(1, 1, sizeof(atsc_mpeg_packet_rs_encoded))),
			last_start(-1)
{
	set_output_multiple(NCODERS);
	debug = true;

	/*
	 * These fifo's handle the alignment problem caused by the
	 * inherent decoding delay of the individual viterbi decoders.
	 * The net result is that this entire block has a pipeline latency
	 * of 12 complete segments.
	 *
	 * If anybody cares, it is possible to do it with less delay, but
	 * this approach is at least somewhat understandable...
	 */

	// the -4 is for the 4 sync symbols
	int fifo_size = ATSC_DATA_SEGMENT_LENGTH - 4 - viterbi[0].delay ();
	for (int i = 0; i < NCODERS; i++)
		fifo[i] = new fifo_t(fifo_size);

	reset();
}

atsc_viterbi_decoder::~atsc_viterbi_decoder ()
{
	for (int i = 0; i < NCODERS; i++)
		delete fifo[i];
}

void
atsc_viterbi_decoder::reset ()
{
	for (int i = 0; i < NCODERS; i++)
	{
		viterbi[i].reset ();
		fifo[i]->reset ();
	}
}

void
atsc_viterbi_decoder::decode_helper (unsigned char out[OUTPUT_SIZE],
				     const float symbols_in[INPUT_SIZE])
{
	int encoder;
	unsigned int i;
	//int dbi;
	int dbwhere;
	int dbindex;
	int shift;
	unsigned char dibit;
	float symbol;

	/* Now run each of the 12 Trellis encoders over their subset of
	the input symbols */
	for (encoder = 0; encoder < NCODERS; encoder++)
	{
		//dbi = 0;			/* Reinitialize dibit index for new encoder */
		fifo_t	*dibit_fifo = fifo[encoder];

		/* Feed all the incoming symbols into one encoder;
		pump them into the relevant dibits. */
		for (i = 0; i < enco_which_max; i++)
		{
			symbol = symbols_in[enco_which_syms[encoder][i]];
			dibit = dibit_fifo->stuff (viterbi[encoder].decode (symbol));
			// printf ("%d\n", dibit);
			/* Store the dibit into the output data segment */
			dbwhere = enco_which_dibits[encoder][i];
			dbindex = dbwhere >> 3;
			shift = dbwhere & 0x7;
			out[dbindex] = (out[dbindex] & ~(0x03 << shift)) | (dibit << shift);
		} /* Symbols fed into one encoder */
	} /* Encoders */
}

int
atsc_viterbi_decoder::work (int noutput_items,
                            gr_vector_const_void_star &input_items,
                            gr_vector_void_star &output_items)
{
	const atsc_soft_data_segment *in = (const atsc_soft_data_segment *) input_items[0];
	atsc_mpeg_packet_rs_encoded *out = (atsc_mpeg_packet_rs_encoded *) output_items[0];

	assert (noutput_items % atsc_viterbi_decoder::NCODERS == 0);

	// The way the fs_checker works ensures we start getting packets starting with 
	// a field sync, and out input multiple is set to 12, so we should always get a
	// mod 12 numbered first packet

	unsigned char out_copy[OUTPUT_SIZE];
	float in_copy[INPUT_SIZE];

	for (int i = 0; i < NCODERS; i += NCODERS)
	{
		// copy input into continguous temporary buffer
		for (int j = 0; j < NCODERS; j++)
		{
			assert (in[i + j].pli.regular_seg_p ());
			memcpy (&in_copy[j * INPUT_SIZE/NCODERS],
			        &in[i + j].data[0],
			        ATSC_DATA_SEGMENT_LENGTH * sizeof (in_copy[0]));
		}

		// do the deed...
		decode_helper (out_copy, in_copy);

		// copy output from contiguous temp buffer into final output
		for (int j = 0; j < NCODERS; j++)
		{
			memcpy (&out[i + j].data[0],
			&out_copy[j * OUTPUT_SIZE/NCODERS],
			ATSC_MPEG_RS_ENCODED_LENGTH * sizeof (out_copy[0]));

			// adjust pipeline info to reflect 12 segment delay
			plinfo::delay (out[i + j].pli, in[i + j].pli, NCODERS);
		}
	}

	return NCODERS;
}


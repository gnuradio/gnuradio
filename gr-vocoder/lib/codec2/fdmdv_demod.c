/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: fdmdv_demod.c
  AUTHOR......: David Rowe  
  DATE CREATED: April 30 2012
                                                                             
  Given an input raw file (8kHz, 16 bit shorts) of FDMDV modem samples
  outputs a file of bits.  The output file is assumed to be arranged
  as codec frames of 56 bits (7 bytes) which are received as two 28
  bit modem frames.

  Demod states can be optionally logged to an Octave file for display
  using the Octave script fdmdv_demod_c.m.  This is useful for
  checking demod performance.
                                                                             
\*---------------------------------------------------------------------------*/


/*
  Copyright (C) 2012 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "codec2_fdmdv.h"
#include "octave.h"

/* lof of information we want to dump to Octave */

#define MAX_FRAMES 50*60 /* 1 minute at 50 symbols/s */

int main(int argc, char *argv[])
{
    FILE         *fin, *fout;
    struct FDMDV *fdmdv;
    char         *packed_bits;
    int          *rx_bits;
    int          *codec_bits;
    COMP          rx_fdm[FDMDV_MAX_SAMPLES_PER_FRAME];
    short         rx_fdm_scaled[FDMDV_MAX_SAMPLES_PER_FRAME];
    int           i, bit, byte, c;
    int           nin, nin_prev;
    int           sync_bit = 0, reliable_sync_bit;
    int           sync = 0;
    int           f;
    FILE         *foct = NULL;
    struct FDMDV_STATS stats;
    COMP         *rx_fdm_log;
    int           rx_fdm_log_col_index;
    COMP         *rx_symbols_log;
    int           sync_log[MAX_FRAMES];
    float         rx_timing_log[MAX_FRAMES];
    float         foff_log[MAX_FRAMES];
    int           sync_bit_log[MAX_FRAMES];
    int           rx_bits_log[FDMDV_BITS_PER_FRAME*MAX_FRAMES];
    float         snr_est_log[MAX_FRAMES];
    float        *rx_spec_log;
    int           max_frames_reached;
    int           bits_per_fdmdv_frame;
    int           bits_per_codec_frame;
    int           bytes_per_codec_frame;
    int           Nc;

    if (argc < 3) {
	printf("usage: %s InputModemRawFile OutputBitFile [Nc] [OctaveDumpFile]\n", argv[0]);
	printf("e.g    %s hts1a_fdmdv.raw hts1a.c2\n", argv[0]);
	exit(1);
    }

    if (strcmp(argv[1], "-")  == 0) fin = stdin;
    else if ( (fin = fopen(argv[1],"rb")) == NULL ) {
	fprintf(stderr, "Error opening input modem sample file: %s: %s.\n",
         argv[1], strerror(errno));
	exit(1);
    }

    if (strcmp(argv[2], "-") == 0) fout = stdout;
    else if ( (fout = fopen(argv[2],"wb")) == NULL ) {
	fprintf(stderr, "Error opening output bit file: %s: %s.\n",
         argv[2], strerror(errno));
	exit(1);
    }

    if (argc >= 4) {
        Nc = atoi(argv[3]);
        if ((Nc % 2) != 0) {
            fprintf(stderr, "Error number of carriers must be a multiple of 2\n");
            exit(1);
        }
        if ((Nc < 2) || (Nc > FDMDV_NC_MAX) ) {
            fprintf(stderr, "Error number of carriers must be btween 2 and %d\n",  FDMDV_NC_MAX);
            exit(1);
        }
    }
    else
        Nc = FDMDV_NC;
    
    fdmdv = fdmdv_create(Nc);

    bits_per_fdmdv_frame = fdmdv_bits_per_frame(fdmdv);
    bits_per_codec_frame = 2*fdmdv_bits_per_frame(fdmdv);
    assert((bits_per_codec_frame % 8) == 0); /* make sure integer number of bytes per frame */
    bytes_per_codec_frame = bits_per_codec_frame/8;

    /* malloc some buffers that are dependant on Nc */

    packed_bits = (char*)malloc(bytes_per_codec_frame); assert(packed_bits != NULL);
    rx_bits = (int*)malloc(sizeof(int)*bits_per_codec_frame); assert(rx_bits != NULL);
    codec_bits = (int*)malloc(2*sizeof(int)*bits_per_fdmdv_frame); assert(codec_bits != NULL);

    /* malloc some of the larger variables to prevent out of stack problems */

    rx_fdm_log = (COMP*)malloc(sizeof(COMP)*FDMDV_MAX_SAMPLES_PER_FRAME*MAX_FRAMES);
    assert(rx_fdm_log != NULL);
    rx_spec_log = (float*)malloc(sizeof(float)*FDMDV_NSPEC*MAX_FRAMES);
    assert(rx_spec_log != NULL);
    rx_symbols_log = (COMP*)malloc(sizeof(COMP)*(Nc+1)*MAX_FRAMES);
    assert(rx_fdm_log != NULL);

    f = 0;
    nin = FDMDV_NOM_SAMPLES_PER_FRAME;
    rx_fdm_log_col_index = 0;
    max_frames_reached = 0;

    while(fread(rx_fdm_scaled, sizeof(short), nin, fin) == nin)
    {
	for(i=0; i<nin; i++) {
	    rx_fdm[i].real = (float)rx_fdm_scaled[i]/FDMDV_SCALE;
            rx_fdm[i].imag = 0;
        }
	nin_prev = nin;
	fdmdv_demod(fdmdv, rx_bits, &reliable_sync_bit, rx_fdm, &nin);

	/* log data for optional Octave dump */

	if (f < MAX_FRAMES) {
	    fdmdv_get_demod_stats(fdmdv, &stats);

	    /* log modem states for later dumping to Octave log file */

	    memcpy(&rx_fdm_log[rx_fdm_log_col_index], rx_fdm, sizeof(COMP)*nin_prev);
	    rx_fdm_log_col_index += nin_prev;

	    for(c=0; c<Nc+1; c++)
		rx_symbols_log[f*(Nc+1)+c] = stats.rx_symbols[c];
	    foff_log[f] = stats.foff;
	    rx_timing_log[f] = stats.rx_timing;
	    sync_log[f] = stats.sync;
	    sync_bit_log[f] = sync_bit;
	    memcpy(&rx_bits_log[bits_per_fdmdv_frame*f], rx_bits, sizeof(int)*bits_per_fdmdv_frame);
	    snr_est_log[f] = stats.snr_est;

	    fdmdv_get_rx_spectrum(fdmdv, &rx_spec_log[f*FDMDV_NSPEC], rx_fdm, nin_prev);

	    f++;
	}
	
	if ((f == MAX_FRAMES) && !max_frames_reached) {
	    fprintf(stderr,"MAX_FRAMES exceed in Octave log, log truncated\n");
	    max_frames_reached = 1;
	}

        if (reliable_sync_bit)
            sync = 1;
        //printf("sync_bit: %d reliable_sync_bit: %d sync: %d\n", sync_bit, reliable_sync_bit, sync);

        if (sync == 0) {
            memcpy(codec_bits, rx_bits, bits_per_fdmdv_frame*sizeof(int));
            sync = 1;
        }
        else {
            memcpy(&codec_bits[bits_per_fdmdv_frame], rx_bits, bits_per_fdmdv_frame*sizeof(int));

            /* pack bits, MSB received first  */

            bit = 7; byte = 0;
            memset(packed_bits, 0, bytes_per_codec_frame);
            for(i=0; i<bits_per_codec_frame; i++) {
                packed_bits[byte] |= (codec_bits[i] << bit);
                bit--;
                if (bit < 0) {
                    bit = 7;
                    byte++;
                }
            }
            assert(byte == bytes_per_codec_frame);

            fwrite(packed_bits, sizeof(char), bytes_per_codec_frame, fout);
            sync = 0;
        }
           

	/* if this is in a pipeline, we probably don't want the usual
	   buffering to occur */

        if (fout == stdout) fflush(stdout);
        if (fin == stdin) fflush(stdin);         
    }

    /* Optional dump to Octave log file */

    if (argc == 5) {

	/* make sure 3rd arg is not just the pipe command */

	if (strcmp(argv[3],"|")) {
	    if ((foct = fopen(argv[3],"wt")) == NULL ) {
		fprintf(stderr, "Error opening Octave dump file: %s: %s.\n",
			argv[3], strerror(errno));
		exit(1);
	    }
	    octave_save_complex(foct, "rx_fdm_log_c", rx_fdm_log, 1, rx_fdm_log_col_index, FDMDV_MAX_SAMPLES_PER_FRAME);  
	    octave_save_complex(foct, "rx_symbols_log_c", (COMP*)rx_symbols_log, Nc+1, f, MAX_FRAMES);  
	    octave_save_float(foct, "foff_log_c", foff_log, 1, f, MAX_FRAMES);  
	    octave_save_float(foct, "rx_timing_log_c", rx_timing_log, 1, f, MAX_FRAMES);  
	    octave_save_int(foct, "sync_log_c", sync_log, 1, f);  
	    octave_save_int(foct, "rx_bits_log_c", rx_bits_log, 1, bits_per_fdmdv_frame*f);
	    octave_save_int(foct, "sync_bit_log_c", sync_bit_log, 1, f);  
	    octave_save_float(foct, "snr_est_log_c", snr_est_log, 1, f, MAX_FRAMES);  
	    octave_save_float(foct, "rx_spec_log_c", rx_spec_log, f, FDMDV_NSPEC, FDMDV_NSPEC);  
	    fclose(foct);
	}
    }

    //fdmdv_dump_osc_mags(fdmdv);

    fclose(fin);
    fclose(fout);
    free(rx_fdm_log);
    free(rx_spec_log);
    fdmdv_destroy(fdmdv);

    return 0;
}


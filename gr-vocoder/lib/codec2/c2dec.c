/*---------------------------------------------------------------------------*\

  FILE........: c2dec.c
  AUTHOR......: David Rowe
  DATE CREATED: 23/8/2010

  Decodes a file of bits to a file of raw speech samples using codec2.

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2010 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include "codec2.h"
#include "dump.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#define NONE          0  /* no bit errors                          */
#define UNIFORM       1  /* random bit errors                      */
#define TWO_STATE     2  /* Two state error model                  */
#define UNIFORM_RANGE 3  /* random bit errors over a certain range */

void print_help(const struct option *long_options, int num_opts, char* argv[]);

int main(int argc, char *argv[])
{
    int            mode;
    void          *codec2;
    FILE          *fin;
    FILE          *fout;
    FILE          *fber = NULL;
    short         *buf;
    unsigned char *bits, *prev_bits;
    int            nsam, nbit, nbyte, i, byte, frames, bits_proc, bit_errors, error_mode;
    int            nstart_bit, nend_bit, bit_rate;
    int            state, next_state;
    float          ber, r, burst_length, burst_period, burst_timer, ber_est;
    unsigned char  mask;
    int            natural, dump;

    char* opt_string = "h:";
    struct option long_options[] = {
        { "ber", required_argument, NULL, 0 },
        { "startbit", required_argument, NULL, 0 },
        { "endbit", required_argument, NULL, 0 },
        { "berfile", required_argument, NULL, 0 },
        { "natural", no_argument, &natural, 1 },
        #ifdef DUMP
        { "dump", required_argument, &dump, 1 },
        #endif
        { "help", no_argument, NULL, 'h' },
        { NULL, no_argument, NULL, 0 }
    };
    int num_opts=sizeof(long_options)/sizeof(struct option);

    if (argc < 4)
        print_help(long_options, num_opts, argv);

    if (strcmp(argv[1],"3200") == 0)
	mode = CODEC2_MODE_3200;
    else if (strcmp(argv[1],"2400") == 0)
	mode = CODEC2_MODE_2400;
    else if (strcmp(argv[1],"1600") == 0)
	mode = CODEC2_MODE_1600;
    else if (strcmp(argv[1],"1400") == 0)
	mode = CODEC2_MODE_1400;
    else if (strcmp(argv[1],"1300") == 0)
	mode = CODEC2_MODE_1300;
    else if (strcmp(argv[1],"1200") == 0)
	mode = CODEC2_MODE_1200;
    else {
	fprintf(stderr, "Error in mode: %s.  Must be 3200, 2400, 1600, 1400, 1300 or 1200\n", argv[1]);
	exit(1);
    }
    bit_rate = atoi(argv[1]);

    if (strcmp(argv[2], "-")  == 0) fin = stdin;
    else if ( (fin = fopen(argv[2],"rb")) == NULL ) {
	fprintf(stderr, "Error opening input bit file: %s: %s.\n",
         argv[2], strerror(errno));
	exit(1);
    }

    if (strcmp(argv[3], "-") == 0) fout = stdout;
    else if ( (fout = fopen(argv[3],"wb")) == NULL ) {
	fprintf(stderr, "Error opening output speech file: %s: %s.\n",
         argv[3], strerror(errno));
	exit(1);
    }

    error_mode = NONE;
    ber = 0.0;
    burst_length = burst_period = 0.0;
    burst_timer = 0.0;
    dump = natural = 0;

    codec2 = codec2_create(mode);
    nsam = codec2_samples_per_frame(codec2);
    nbit = codec2_bits_per_frame(codec2);
    buf = (short*)malloc(nsam*sizeof(short));
    nbyte = (nbit + 7) / 8;
    bits = (unsigned char*)malloc(nbyte*sizeof(char));
    prev_bits = (unsigned char*)malloc(nbyte*sizeof(char));
    frames = bit_errors = bits_proc = 0;
    nstart_bit = 0;
    nend_bit = nbit-1;

    while(1) {
        int option_index = 0;
        int opt = getopt_long(argc, argv, opt_string,
                    long_options, &option_index);
        if (opt == -1)
            break;

        switch (opt) {
        case 0:
            if(strcmp(long_options[option_index].name, "ber") == 0) {
                ber = atof(optarg);
                error_mode = UNIFORM;
            } else if(strcmp(long_options[option_index].name, "startbit") == 0) {
	        nstart_bit = atoi(optarg);
            } else if(strcmp(long_options[option_index].name, "endbit") == 0) {
	        nend_bit = atoi(optarg);
             } else if(strcmp(long_options[option_index].name, "berfile") == 0) {
	        if ((fber = fopen(optarg,"wt")) == NULL) {
	            fprintf(stderr, "Error opening BER file: %s %s.\n",
                            optarg, strerror(errno));
                    exit(1);
                }

            }
            #ifdef DUMP
            else if(strcmp(long_options[option_index].name, "dump") == 0) {
                if (dump)
	            dump_on(optarg);
            }
            #endif
            break;

        case 'h':
            print_help(long_options, num_opts, argv);
            break;

        default:
            /* This will never be reached */
            break;
        }
    }
    assert(nend_bit <= nbit);
    codec2_set_natural_or_gray(codec2, !natural);
    //printf("%d %d\n", nstart_bit, nend_bit);

    while(fread(bits, sizeof(char), nbyte, fin) == (size_t)nbyte) {
	frames++;

        // apply bit errors, MSB of byte 0 is bit 0 in frame */

	if ((error_mode == UNIFORM) || (error_mode == UNIFORM_RANGE)) {
	    for(i=nstart_bit; i<nend_bit+1; i++) {
		r = (float)rand()/RAND_MAX;
		if (r < ber) {
		    byte = i/8;
		    //printf("nbyte %d nbit %d i %d byte %d bits[%d] 0x%0x ", nbyte, nbit, i, byte, byte, bits[byte]);
		    mask = 1 << (7 - i + byte*8);
                    bits[byte] ^= mask;
		    //printf("shift: %d mask: 0x%0x bits[%d] 0x%0x\n", 7 - i + byte*8, mask, byte, bits[byte] );
		    bit_errors++;
 		}
                bits_proc++;
	    }
	}

	if (error_mode == TWO_STATE) {
            burst_timer += (float)nbit/bit_rate;
            fprintf(stderr, "burst_timer: %f  state: %d\n", burst_timer, state);

            next_state = state;
            switch(state) {
            case 0:

                /* clear channel state - no bit errors */

                if (burst_timer > (burst_period - burst_length))
                    next_state = 1;
                break;

            case 1:

                /* burst error state - 50% bit error rate */

                for(i=nstart_bit; i<nend_bit+1; i++) {
                    r = (float)rand()/RAND_MAX;
                    if (r < 0.5) {
                        byte = i/8;
                        bits[byte] ^= 1 << (7 - i + byte*8);
                        bit_errors++;
                    }
                    bits_proc++;
		}

                if (burst_timer > burst_period) {
                    burst_timer = 0.0;
                    next_state = 0;
                }
                break;

	    }

            state = next_state;
        }

        if (fber != NULL) {
            if (fread(&ber_est, sizeof(float), 1, fber) != 1) {
                fprintf(stderr, "ran out of BER estimates!\n");
                exit(1);
            }
            //fprintf(stderr, "ber_est: %f\n", ber_est);
        }
        else
            ber_est = 0.0;

	codec2_decode_ber(codec2, buf, bits, ber_est);
 	fwrite(buf, sizeof(short), nsam, fout);
	//if this is in a pipeline, we probably don't want the usual
        //buffering to occur
        if (fout == stdout) fflush(stdout);
        if (fin == stdin) fflush(stdin);

        memcpy(prev_bits, bits, nbyte);
    }

    if (error_mode)
	fprintf(stderr, "actual BER: %1.3f\n", (float)bit_errors/bits_proc);

    codec2_destroy(codec2);

    free(buf);
    free(bits);
    fclose(fin);
    fclose(fout);

    return 0;
}

void print_help(const struct option* long_options, int num_opts, char* argv[])
{
	int i;
	char *option_parameters;
	fprintf(stderr, "\nc2dec - Codec 2 decoder and bit error simulation program\n"
		"usage: %s 3200|2400|1400}1300|1200 InputFile OutputRawFile [OPTIONS]\n\n"
                "Options:\n", argv[0]);
        for(i=0; i<num_opts-1; i++) {
		if(long_options[i].has_arg == no_argument) {
			option_parameters="";
		} else if (strcmp("ber", long_options[i].name) == 0) {
			option_parameters = " BER";
		} else if (strcmp("startbit", long_options[i].name) == 0) {
			option_parameters = " startBit";
		} else if (strcmp("endbit", long_options[i].name) == 0) {
			option_parameters = " endBit";
		} else if (strcmp("berfile", long_options[i].name) == 0) {
			option_parameters = " berFileName";
		} else if (strcmp("dump", long_options[i].name) == 0) {
			option_parameters = " dumpFilePrefix";
		} else {
			option_parameters = " <UNDOCUMENTED parameter>";
		}
		fprintf(stderr, "\t--%s%s\n", long_options[i].name, option_parameters);
	}
	exit(1);
}

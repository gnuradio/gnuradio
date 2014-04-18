/*---------------------------------------------------------------------------*\

  FILE........: insert_errors.c
  AUTHOR......: David Rowe
  DATE CREATED: 20/2/2013

  Inserts errors into a Codec 2 bit stream using error files.  The
  error files have one 16 bit short per bit, the short is set to 1 if
  there is an error, or zero otherwise.  The Codec 2 bit stream files
  are in packed format, i.e. c2enc/c2dec format.

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2013 David Rowe

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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    FILE          *fin;
    FILE          *fout;
    FILE          *ferror;
    int            i, start_bit, end_bit, bit;
    unsigned char  byte;
    short          error;
    int            errors, bits;
    int            bits_per_frame;

    if (argc < 4) {
	printf("%s InputBitFile OutputBitFile ErrorFile bitsPerFrame [startBit endBit]\n", argv[0]);
	printf("%s InputBitFile OutputBitFile BER\n", argv[0]);
	exit(1);
    }

    if (strcmp(argv[1], "-")  == 0) fin = stdin;
    else if ( (fin = fopen(argv[1],"rb")) == NULL ) {
	fprintf(stderr, "Error opening input bit file: %s: %s.\n",
         argv[1], strerror(errno));
	exit(1);
    }

    if (strcmp(argv[2], "-") == 0) fout = stdout;
    else if ( (fout = fopen(argv[2],"wb")) == NULL ) {
	fprintf(stderr, "Error opening output speech file: %s: %s.\n",
         argv[2], strerror(errno));
	exit(1);
    }

    if ((ferror = fopen(argv[3],"rb")) == NULL ) {
	fprintf(stderr, "Error opening error file: %s: %s.\n",
         argv[3], strerror(errno));
	exit(1);
    }

    bits_per_frame = atoi(argv[4]);
    assert((bits_per_frame % 8) == 0);

    start_bit = 0; end_bit = bits_per_frame;
    if (argc == 7) {
 	start_bit = atoi(argv[5]);
	end_bit = atoi(argv[6]);   
    }
        
    bit = 0;
    bits = errors = 0;

    while(fread(&byte, sizeof(char), 1, fin) == 1) {

        for(i=0; i<8; i++) {
            bits++;
            //printf("bit: %d start_bit: %d end_bit: %d\n", bit, start_bit, end_bit);
            if (fread(&error, sizeof(short), 1, ferror)) {
                if ((bit >= start_bit) && (bit <= end_bit))
                    byte ^= error << i;
                if (error)
                    errors++;
            }
            else {
                fprintf(stderr,"bits: %d ber: %4.3f\n", bits, (float)errors/bits);
                fclose (fin); fclose(fout); fclose(ferror);
                exit(0);
            }
            bit++;
            if (bit == bits_per_frame)
                bit = 0;
        }
        fwrite(&byte, sizeof(char), 1, fout);
        if (fout == stdout) fflush(stdout);
        if (fin == stdin) fflush(stdin);         
 
    }

    fclose(fin);
    fclose(fout);
    fclose(ferror);

    fprintf(stderr,"bits: %d ber: %4.3f\n", bits, (float)errors/bits);

    return 0;
}

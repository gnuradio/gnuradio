/*
 * decode.c
 *
 * CCITT ADPCM decoder
 *
 * Usage : decode [-3|4|5] [-a|u|l] < infile > outfile
 */
#include <stdio.h>
#include "g72x.h"


/*
 * Unpack input codes and pass them back as bytes.
 * Returns 1 if there is residual input, returns -1 if eof, else returns 0.
 */
int
unpack_input(
	unsigned char		*code,
	int			bits)
{
	static unsigned int	in_buffer = 0;
	static int		in_bits = 0;
	unsigned char		in_byte;

	if (in_bits < bits) {
		if (fread(&in_byte, sizeof (char), 1, stdin) != 1) {
			*code = 0;
			return (-1);
		}
		in_buffer |= (in_byte << in_bits);
		in_bits += 8;
	}
	*code = in_buffer & ((1 << bits) - 1);
	in_buffer >>= bits;
	in_bits -= bits;
	return (in_bits > 0);
}


main(
	int			argc,
	char			**argv)
{
	short			sample;
	unsigned char		code;
	int			n;
	struct g72x_state	state;
	int			out_coding;
	int			out_size;
	int			(*dec_routine)();
	int			dec_bits;

	g72x_init_state(&state);
	out_coding = AUDIO_ENCODING_ULAW;
	out_size = sizeof (char);
	dec_routine = g721_decoder;
	dec_bits = 4;

	/* Process encoding argument, if any */
	while ((argc > 1) && (argv[1][0] == '-')) {
		switch (argv[1][1]) {
		case '3':
			dec_routine = g723_24_decoder;
			dec_bits = 3;
			break;
		case '4':
			dec_routine = g721_decoder;
			dec_bits = 4;
			break;
		case '5':
			dec_routine = g723_40_decoder;
			dec_bits = 5;
			break;
		case 'u':
			out_coding = AUDIO_ENCODING_ULAW;
			out_size = sizeof (char);
			break;
		case 'a':
			out_coding = AUDIO_ENCODING_ALAW;
			out_size = sizeof (char);
			break;
		case 'l':
			out_coding = AUDIO_ENCODING_LINEAR;
			out_size = sizeof (short);
			break;
		default:
fprintf(stderr, "CCITT ADPCM Decoder -- usage:\n");
fprintf(stderr, "\tdecode [-3|4|5] [-a|u|l] < infile > outfile\n");
fprintf(stderr, "where:\n");
fprintf(stderr, "\t-3\tProcess G.723 24kbps (3-bit) input data\n");
fprintf(stderr, "\t-4\tProcess G.721 32kbps (4-bit) input data [default]\n");
fprintf(stderr, "\t-5\tProcess G.723 40kbps (5-bit) input data\n");
fprintf(stderr, "\t-a\tGenerate 8-bit A-law data\n");
fprintf(stderr, "\t-u\tGenerate 8-bit u-law data [default]\n");
fprintf(stderr, "\t-l\tGenerate 16-bit linear PCM data\n");
			exit(1);
		}
		argc--;
		argv++;
	}

	/* Read and unpack input codes and process them */
	while (unpack_input(&code, dec_bits) >= 0) {
		sample = (*dec_routine)(code, out_coding, &state);
		if (out_size == 2) {
			fwrite(&sample, out_size, 1, stdout);
		} else {
			code = (unsigned char)sample;
			fwrite(&code, out_size, 1, stdout);
		}
	}
	fclose(stdout);
}

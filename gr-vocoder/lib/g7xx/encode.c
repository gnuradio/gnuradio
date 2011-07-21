/*
 * encode.c
 *
 * CCITT ADPCM encoder
 *
 * Usage : encode [-3|4|5] [-a|u|l] < infile > outfile
 */
#include <stdio.h>
#include "g72x.h"


/*
 * Pack output codes into bytes and write them to stdout.
 * Returns 1 if there is residual output, else returns 0.
 */
int
pack_output(
	unsigned		code,
	int			bits)
{
	static unsigned int	out_buffer = 0;
	static int		out_bits = 0;
	unsigned char		out_byte;

	out_buffer |= (code << out_bits);
	out_bits += bits;
	if (out_bits >= 8) {
		out_byte = out_buffer & 0xff;
		out_bits -= 8;
		out_buffer >>= 8;
		fwrite(&out_byte, sizeof (char), 1, stdout);
	}
	return (out_bits > 0);
}


main(
	int			argc,
	char			**argv)
{
	struct g72x_state	state;
	unsigned char		sample_char;
	short			sample_short;
	unsigned char		code;
	int			resid;
	int			in_coding;
	int			in_size;
	unsigned		*in_buf;
	int			(*enc_routine)();
	int			enc_bits;

	g72x_init_state(&state);

	/* Set defaults to u-law input, G.721 output */
	in_coding = AUDIO_ENCODING_ULAW;
	in_size = sizeof (char);
	in_buf = (unsigned *)&sample_char;
	enc_routine = g721_encoder;
	enc_bits = 4;

	/* Process encoding argument, if any */
	while ((argc > 1) && (argv[1][0] == '-')) {
		switch (argv[1][1]) {
		case '3':
			enc_routine = g723_24_encoder;
			enc_bits = 3;
			break;
		case '4':
			enc_routine = g721_encoder;
			enc_bits = 4;
			break;
		case '5':
			enc_routine = g723_40_encoder;
			enc_bits = 5;
			break;
		case 'u':
			in_coding = AUDIO_ENCODING_ULAW;
			in_size = sizeof (char);
			in_buf = (unsigned *)&sample_char;
			break;
		case 'a':
			in_coding = AUDIO_ENCODING_ALAW;
			in_size = sizeof (char);
			in_buf = (unsigned *)&sample_char;
			break;
		case 'l':
			in_coding = AUDIO_ENCODING_LINEAR;
			in_size = sizeof (short);
			in_buf = (unsigned *)&sample_short;
			break;
		default:
fprintf(stderr, "CCITT ADPCM Encoder -- usage:\n");
fprintf(stderr, "\tencode [-3|4|5] [-a|u|l] < infile > outfile\n");
fprintf(stderr, "where:\n");
fprintf(stderr, "\t-3\tGenerate G.723 24kbps (3-bit) data\n");
fprintf(stderr, "\t-4\tGenerate G.721 32kbps (4-bit) data [default]\n");
fprintf(stderr, "\t-5\tGenerate G.723 40kbps (5-bit) data\n");
fprintf(stderr, "\t-a\tProcess 8-bit A-law input data\n");
fprintf(stderr, "\t-u\tProcess 8-bit u-law input data [default]\n");
fprintf(stderr, "\t-l\tProcess 16-bit linear PCM input data\n");
			exit(1);
		}
		argc--;
		argv++;
	}

	/* Read input file and process */
	while (fread(in_buf, in_size, 1, stdin) == 1) {
	  code = (*enc_routine)(in_size == 2 ? sample_short : sample_char,
		    in_coding, &state);
		resid = pack_output(code, enc_bits);
	}

	/* Write zero codes until all residual codes are written out */
	while (resid) {
		resid = pack_output(0, enc_bits);
	}
	fclose(stdout);
}

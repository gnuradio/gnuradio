/* User include file for the Reed-Solomon codec
 * Copyright 2002, Phil Karn KA9Q
 * May be used under the terms of the GNU General Public License (GPL)
 */

#ifndef INCLUDED_RS_H
#define INCLUDED_RS_H
#include <gnuradio/fec/api.h>

/* General purpose RS codec, 8-bit symbols */
FEC_API void encode_rs_char(void* rs, unsigned char* data, unsigned char* parity);
FEC_API int decode_rs_char(void* rs, unsigned char* data, int* eras_pos, int no_eras);
FEC_API void* init_rs_char(unsigned int symsize,
                           unsigned int gfpoly,
                           unsigned int fcr,
                           unsigned int prim,
                           unsigned int nroots);
FEC_API void free_rs_char(void* rs);

/* CCSDS standard (255,223) RS codec with conventional (*not* dual-basis)
 * symbol representation
 */
FEC_API void encode_rs_8(unsigned char* data, unsigned char* parity);
FEC_API int decode_rs_8(unsigned char* data, int* eras_pos, int no_eras);

/* CCSDS standard (255,223) RS codec with dual-basis symbol representation
 */
FEC_API void encode_rs_ccsds(unsigned char* data, unsigned char* parity);
FEC_API int decode_rs_ccsds(unsigned char* data, int* eras_pos, int no_eras);

#endif /* INCLUDED_RS_H */

/* User include file for the Reed-Solomon codec
 * Copyright 2002, Phil Karn KA9Q
 * May be used under the terms of the GNU General Public License (GPL)
 */

/* General purpose RS codec, 8-bit symbols */
void encode_rs_char(void *rs,unsigned char *data,unsigned char *parity);
int decode_rs_char(void *rs,unsigned char *data,int *eras_pos,
		   int no_eras);
void *init_rs_char(unsigned int symsize,unsigned int gfpoly,
		   unsigned int fcr,unsigned int prim,unsigned int nroots);
void free_rs_char(void *rs);

/* General purpose RS codec, integer symbols */
void encode_rs_int(void *rs,int *data,int *parity);
int decode_rs_int(void *rs,int *data,int *eras_pos,int no_eras);
void *init_rs_int(unsigned int symsize,unsigned int gfpoly,unsigned int fcr,
		  unsigned int prim,unsigned int nroots);
void free_rs_int(void *rs);

/* CCSDS standard (255,223) RS codec with conventional (*not* dual-basis)
 * symbol representation
 */
void encode_rs_8(unsigned char *data,unsigned char *parity);
int decode_rs_8(unsigned char *data,int *eras_pos,int no_eras);

/* Tables to map from conventional->dual (Taltab) and
 * dual->conventional (Tal1tab) bases
 */
extern unsigned char Taltab[],Tal1tab[];

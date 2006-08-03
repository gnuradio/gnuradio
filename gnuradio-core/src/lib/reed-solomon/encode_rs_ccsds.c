/* This function wraps around the fixed 8-bit encoder, performing the
 * basis transformations necessary to meet the CCSDS standard
 *
 * Copyright 2002, Phil Karn, KA9Q
 * May be used under the terms of the GNU General Public License (GPL)
 */
#define FIXED
#include "fixed.h"
#include "ccsds.h"

void encode_rs_ccsds(unsigned char *data,unsigned char *parity){
  int i;
  unsigned char cdata[NN-NROOTS];

  /* Convert data from dual basis to conventional */
  for(i=0;i<NN-NROOTS;i++)
    cdata[i] = Tal1tab[data[i]];

  encode_rs_8(cdata,parity);

  /* Convert parity from conventional to dual basis */
  for(i=0;i<NN-NROOTS;i++)
    parity[i] = Taltab[parity[i]];
}

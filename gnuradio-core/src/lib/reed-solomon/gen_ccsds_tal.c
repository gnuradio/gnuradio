/* Conversion lookup tables from conventional alpha to Berlekamp's
 * dual-basis representation. Used in the CCSDS version only.
 * taltab[] -- convert conventional to dual basis
 * tal1tab[] -- convert dual basis to conventional

 * Note: the actual RS encoder/decoder works with the conventional basis.
 * So data is converted from dual to conventional basis before either
 * encoding or decoding and then converted back.
 *
 * Copyright 2002 Phil Karn, KA9Q
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include <stdio.h>
unsigned char Taltab[256],Tal1tab[256];

static unsigned char tal[] = { 0x8d, 0xef, 0xec, 0x86, 0xfa, 0x99, 0xaf, 0x7b };

/* Generate conversion lookup tables between conventional alpha representation
 * (@**7, @**6, ...@**0)
 *  and Berlekamp's dual basis representation
 * (l0, l1, ...l7)
 */
int main(){
  int i,j,k;

  for(i=0;i<256;i++){/* For each value of input */
    Taltab[i] = 0;
    for(j=0;j<8;j++) /* for each column of matrix */
      for(k=0;k<8;k++){ /* for each row of matrix */
	if(i & (1<<k))
	   Taltab[i] ^= tal[7-k] & (1<<j);
      }
    Tal1tab[Taltab[i]] = i;
  }
  printf("unsigned char Taltab[] = {\n");
  for(i=0;i<256;i++){
    if((i % 16) == 0)
      printf("\n");
    printf("0x%02x,",Taltab[i]);
  }
  printf("\n};\n\nunsigned char Tal1tab[] = {");
  for(i=0;i<256;i++){
    if((i % 16) == 0)
      printf("\n");
    printf("0x%02x,",Tal1tab[i]);
  }
  printf("\n};\n");
  exit(0);
}


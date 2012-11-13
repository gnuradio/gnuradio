/* Generate tables for CCSDS code
 * Copyright 2002 Phil Karn, KA9Q
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include <stdio.h>
#include "char.h"

int main(){
  struct rs *rs;
  int i;

  rs = init_rs_char(8,0x187,112,11,32); /* CCSDS standard */
  printf("unsigned char CCSDS_alpha_to[] = {");
  for(i=0;i<256;i++){
    if((i % 16) == 0)
      printf("\n");
    printf("0x%02x,",rs->alpha_to[i]);
  }
  printf("\n};\n\nunsigned char CCSDS_index_of[] = {");
  for(i=0;i<256;i++){
    if((i % 16) == 0)
      printf("\n");
    printf("%3d,",rs->index_of[i]);
  }
  printf("\n};\n\nunsigned char CCSDS_poly[] = {");
  for(i=0;i<33;i++){
    if((i % 16) == 0)
      printf("\n");

    printf("%3d,",rs->genpoly[i]);
  }
  printf("\n};\n");
  exit(0);
}

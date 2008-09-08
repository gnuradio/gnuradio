
#include "memory_map.h"

int main() {
  char *p = (char *)0x4000;
  short *q = (short *)0x5000;
  int *r= (int *)0x6000;

  int *output = (int *)0x7000;

  char s;
  short t;
  int u;

  // Write
  // Bytes
  *p = (char)1;
  p++;
  *p = (char)2;
  p++;
  *p = (char)3;
  p++; 
  *p = (char)4;
  p++;
  *p = (char)5;

  // Words
  *q = (short) 0x1112;
  q++;
  *q = (short) 0x1314;
  q++;
  *q = (short) 0x1516;

  // Double Words
  *r = 0x21222324;
  r++;
  *r = 0x25262728;
  r++;
  *r = 0x292a2b2c;


  // Read
  p = (char *)0x6000;
  s = *p;
  if(s == 0x21)
    *output = 0x53534150;  // PASS
  else
    *output = 0x4C494146;  // FAIL

  p = (char *)0x6001;
  s = *p;
  if(s == 0x22)
    *output = 0x53534150;  // PASS
  else
    *output = 0x4C494146;  // FAIL

  p = (char *)0x6002;
  s = *p;
  if(s == 0x23)
    *output = 0x53534150;  // PASS
  else
    *output = 0x4C494146;  // FAIL


  p = (char *)0x6003;
  s = *p;
  if(s == 0x24)
    *output = 0x53534150;  // PASS
  else
    *output = 0x4C494146;  // FAIL

  q = (short *)0x4000;
  t = *q;
  if(t == 0x0102)
    *output = 0x53534150;  // PASS
  else
    *output = 0x4C494146;  // FAIL

  r = (int *)0x4000;
  u = *r;
  if(u == 0x01020304)
    *output = 0x53534150;  // PASS
  else
    *output = 0x4C494146;  // FAIL

}

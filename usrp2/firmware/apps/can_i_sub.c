#include <u2_init.h>
#include <nonstdio.h>

//typedef long long int64_t;


int64_t sub(int64_t a, int64_t b);
void print(int64_t d);

int main(void)
{
  u2_init();

  int64_t d = sub(462550990848000LL, 462028800000000LL);
  print_uint64(d);
  newline();
  return 0;
}

int64_t sub(int64_t a, int64_t b)
{
  return a - b;
}



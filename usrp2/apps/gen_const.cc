#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
  if (argc != 3){
    fprintf(stderr, "usage: %s i-val q-val\n", argv[0]);
    return 1;
  }

  int i_val = strtol(argv[1], 0, 0);
  int q_val = strtol(argv[2], 0, 0);

  static const int NSAMPLES = 16384;

  uint32_t	sample[NSAMPLES];
  sample[0] = ((i_val & 0xffff) << 16) | (q_val & 0xffff);
  for (int i = 1; i < NSAMPLES; i++)
    sample[i] = sample[0];

  while(1){
    write(1, sample, sizeof(sample));
  }
}

#include<volk_rank_archs.h>
#include<stdio.h>

unsigned int volk_rank_archs(const int* arch_defs, unsigned int arch) {
  int i = 2;
  unsigned int best_val = 0;
  for(; i < arch_defs[0] + 1; ++i) {
    if((arch_defs[i]&(!arch)) == 0) {
      best_val = (arch_defs[i] > arch_defs[best_val + 1]) ? i-1 : best_val;
    }
  }
  return best_val;
}

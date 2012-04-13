#include <volk/volk.h>
#include <qa_16s_branch_4_state_8_aligned16.h>
#include <cstdlib>
#include <ctime>

//test for ssse3

#ifndef LV_HAVE_SSSE3

void qa_16s_branch_4_state_8_aligned16::t1() {
  printf("ssse3 not available... no test performed\n");
}

#else

void qa_16s_branch_4_state_8_aligned16::t1() {
  const int num_iters = 1000000;
  const int vlen = 32;

  static char permute0[16]__attribute__((aligned(16))) = {0x0e, 0x0f, 0x0a, 0x0b, 0x04, 0x05, 0x00, 0x01, 0x0c, 0x0d, 0x08, 0x09, 0x06, 0x07, 0x02, 0x03};
  static char permute1[16]__attribute__((aligned(16))) = {0x0c, 0x0d, 0x08, 0x09, 0x06, 0x07, 0x02, 0x03, 0x0e, 0x0f, 0x0a, 0x0b, 0x04, 0x05, 0x00, 0x01};
  static char permute2[16]__attribute__((aligned(16))) = {0x02, 0x03, 0x06, 0x07, 0x08, 0x09, 0x0c, 0x0d, 0x00, 0x01, 0x04, 0x05, 0x0a, 0x0b, 0x0e, 0x0f};
  static char permute3[16]__attribute__((aligned(16))) = {0x00, 0x01, 0x04, 0x05, 0x0a, 0x0b, 0x0e, 0x0f, 0x02, 0x03, 0x06, 0x07, 0x08, 0x09, 0x0c, 0x0d};
  static char* permuters[4] = {permute0, permute1, permute2, permute3};

  unsigned int num_bytes = vlen << 1;

  volk_environment_init();
  clock_t start, end;
  double total;

  __VOLK_ATTR_ALIGNED(16) short target[vlen];
  __VOLK_ATTR_ALIGNED(16) short target2[vlen];
  __VOLK_ATTR_ALIGNED(16) short target3[vlen];

  __VOLK_ATTR_ALIGNED(16) short src0[vlen];
  __VOLK_ATTR_ALIGNED(16) short permute_indexes[vlen] =  {
7, 5, 2, 0, 6, 4, 3, 1, 6, 4, 3, 1, 7, 5, 2, 0, 1, 3, 4, 6, 0, 2, 5, 7, 0, 2, 5, 7, 1, 3, 4, 6 };
  __VOLK_ATTR_ALIGNED(16) short cntl0[vlen] = {
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
  __VOLK_ATTR_ALIGNED(16) short cntl1[vlen] = {
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
  __VOLK_ATTR_ALIGNED(16) short cntl2[vlen] = {
    0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000 };
  __VOLK_ATTR_ALIGNED(16) short cntl3[vlen] =  {
    0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff };
  __VOLK_ATTR_ALIGNED(16) short scalars[4] = {1, 2, 3, 4};



  for(int i = 0; i < vlen; ++i) {
    src0[i] = i;

  }


  printf("16s_branch_4_state_8_aligned\n");


  start = clock();
  for(int i = 0; i < num_iters; ++i) {
    volk_16s_permute_and_scalar_add_aligned16_manual(target, src0, permute_indexes, cntl0, cntl1, cntl2, cntl3, scalars, num_bytes, "sse2");
  }
  end = clock();

  total = (double)(end-start)/(double)CLOCKS_PER_SEC;

  printf("permute_and_scalar_add_time: %f\n", total);



  start = clock();
  for(int i = 0; i < num_iters; ++i) {
    volk_16s_branch_4_state_8_aligned16_manual(target2, src0, permuters, cntl2, cntl3, scalars, "ssse3");
  }
  end = clock();

  total = (double)(end-start)/(double)CLOCKS_PER_SEC;

  printf("branch_4_state_8_time, ssse3: %f\n", total);

  start = clock();
  for(int i = 0; i < num_iters; ++i) {
    volk_16s_branch_4_state_8_aligned16_manual(target3, src0, permuters, cntl2, cntl3, scalars, "generic");
  }
  end = clock();

  total = (double)(end-start)/(double)CLOCKS_PER_SEC;

  printf("permute_and_scalar_add_time, generic: %f\n", total);



  for(int i = 0; i < vlen; ++i) {
    printf("psa... %d, b4s8... %d\n", target[i], target3[i]);
  }

  for(int i = 0; i < vlen; ++i) {

    CPPUNIT_ASSERT(target[i] == target2[i]);
    CPPUNIT_ASSERT(target[i] == target3[i]);
  }
}


#endif

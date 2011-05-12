#ifndef INCLUDED_VOLK_RANK_ARCHS_H
#define INCLUDED_VOLK_RANK_ARCHS_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned int get_index(const char *indices[], unsigned int n_archs, const char *arch_name);
unsigned int volk_rank_archs(const char *indices[], const int* arch_defs, unsigned int n_archs, const char *name, unsigned int arch);

#ifdef __cplusplus
}
#endif
#endif /*INCLUDED_VOLK_RANK_ARCHS_H*/

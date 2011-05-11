#ifndef INCLUDED_VOLK_RANK_ARCHS_H
#define INCLUDED_VOLK_RANK_ARCHS_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned int volk_rank_archs(const int* arch_defs, unsigned int n_archs, unsigned int arch);

////////////////////////////////////////////////////////////////////////
//get path to volk_config profiling info
////////////////////////////////////////////////////////////////////////
void get_config_path(char *);
void load_preferences(void); //FIXME DEBUG shouldn't be exported

#ifdef __cplusplus
}
#endif
#endif /*INCLUDED_VOLK_RANK_ARCHS_H*/

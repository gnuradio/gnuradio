#include <volk_rank_archs.h>
#include <volk/volk_prefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int get_index(const char *indices[], unsigned int n_archs, const char *arch_name) {
    unsigned int i;
    for(i=0; i<n_archs; i++) {
        if(!strncmp(indices[i], arch_name, 20)) {
            return i;
        }
    }
    //something terrible should happen here
    printf("Volk warning: no arch found, returning generic impl\n");
    return get_index(indices, n_archs, "generic"); //but we'll fake it for now
}

unsigned int volk_rank_archs(const char *indices[], const int* arch_defs, unsigned int n_archs, const char* name, unsigned int arch) {
  unsigned int i;
  unsigned int best_val = 0;
  static struct volk_arch_pref *volk_arch_prefs;
  static unsigned int n_arch_prefs = 0;
  static int prefs_loaded = 0;
  if(!prefs_loaded) {
      n_arch_prefs = load_preferences(&volk_arch_prefs);
      prefs_loaded = 1;
  }
  
  //now look for the function name in the prefs list
  for(i=0; i < n_arch_prefs; i++) {
      if(!strncmp(name, volk_arch_prefs[i].name, 128)) { //found it
        return get_index(indices, n_archs, volk_arch_prefs[i].arch);
      }
  }
  
  for(i=1; i < n_archs; ++i) {
    if((arch_defs[i]&(!arch)) == 0) {
      best_val = (arch_defs[i] > arch_defs[best_val + 1]) ? i-1 : best_val;
    }
  }
  return best_val;
}

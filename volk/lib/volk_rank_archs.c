#include <volk_rank_archs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <Windows.h>
#endif

void get_config_path(char *path) {
    const char *suffix = "/.gnuradio/volk_config";
    memcpy(path, getenv("HOME"), strlen(getenv("HOME"))+1);
    strcat(path, suffix);
}

/*
 * ok so volk stuff has to be loaded piecemeal, and to avoid reading
 * the whole config file in at startup we should probably create a static
 * prefs struct that can be read in by rank_archs with minimal modification.
 * this makes rank_archs slower and load_preferences more complex, but
 * we don't have to export load_preferences and we don't have to include volk.h.
 * means we need to pass the name into rank_archs, though
 * problem is that names don't appear anywhere in the volk function descriptor.
 * so we have to modify things to include the name in the descriptor.
 * 
 * also means you don't have to also spec the fn name in qa_utils.h/c, you can
 * pass it in along with the func_desc
 * 
 */
 
void load_preferences(void) {
    static int prefs_loaded = 0;
    FILE *config_file;
    char path[512], line[512], function[256], arch[64];

    if(prefs_loaded) return;
    
    int n_arch_preferences = 0;
    
    //get the config path
    get_config_path(path);
    config_file = fopen(path, "r");
    if(!config_file) return; //no prefs found

    while(fgets(line, 512, config_file) != NULL) {
        if(sscanf(line, "%s %s", function, arch) == 2 && !strncmp(function, "volk_", 5)) {
            printf("func: %s, arch: %s\n", function, arch);
            //we have a function and we have an arch, let's set it
            n_arch_preferences++;
        }
    }

    fclose(config_file);
    
    printf("Found %d prefs\n", n_arch_preferences);
    prefs_loaded = 1;
}

unsigned int volk_rank_archs(const int* arch_defs, unsigned int n_archs, unsigned int arch) {
  int i = 1;
  unsigned int best_val = 0;
  for(; i < n_archs; ++i) {
    if((arch_defs[i]&(!arch)) == 0) {
      best_val = (arch_defs[i] > arch_defs[best_val + 1]) ? i-1 : best_val;
    }
  }
  return best_val;
}

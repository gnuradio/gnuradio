#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <volk/volk_prefs.h>

//#if defined(_WIN32)
//#include <Windows.h>
//#endif

void get_config_path(char *path) {
    const char *suffix = "/.volk/volk_config";
    char *home = NULL;
    if (home == NULL) home = getenv("HOME");
    if (home == NULL) home = getenv("APPDATA");
    if (home == NULL){
        path = NULL;
        return;
    }
    strcpy(path, home);
    strcat(path, suffix);
}

//passing by reference in C can (***********)
int load_preferences(struct volk_arch_pref **prefs) {
    FILE *config_file;
    char path[512], line[512], function[128], arch[32];
    int n_arch_prefs = 0;
    struct volk_arch_pref *t_pref;
    
    //get the config path
    get_config_path(path);
    if (path == NULL) return n_arch_prefs; //no prefs found
    config_file = fopen(path, "r");
    if(!config_file) return n_arch_prefs; //no prefs found

    while(fgets(line, 512, config_file) != NULL) {
        if(sscanf(line, "%s %s", function, arch) == 2 && !strncmp(function, "volk_", 5)) {
            n_arch_prefs++;
        }
    }

    //now allocate the memory required for volk_arch_prefs
    (*prefs) = (struct volk_arch_pref *) malloc(n_arch_prefs * sizeof(struct volk_arch_pref));
    t_pref = (*prefs);

    //reset the file pointer and write the prefs into volk_arch_prefs
    rewind(config_file);
    while(fgets(line, 512, config_file) != NULL) {
        if(sscanf(line, "%s %s", function, arch) == 2 && !strncmp(function, "volk_", 5)) {
            strncpy(t_pref->name, function, 128);
            strncpy(t_pref->arch, arch, 32);
            t_pref++;
        }
    }
    fclose(config_file);
    return n_arch_prefs;
}

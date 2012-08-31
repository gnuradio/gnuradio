#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <volk/volk_prefs.h>

//#if defined(_WIN32)
//#include <Windows.h>
//#endif

void volk_get_config_path(char *path)
{
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

size_t volk_load_preferences(volk_arch_pref_t **prefs_res)
{
    FILE *config_file;
    char path[512], line[512];
    size_t n_arch_prefs = 0;
    volk_arch_pref_t *prefs = NULL;

    //get the config path
    volk_get_config_path(path);
    if (path == NULL) return n_arch_prefs; //no prefs found
    config_file = fopen(path, "r");
    if(!config_file) return n_arch_prefs; //no prefs found

    //reset the file pointer and write the prefs into volk_arch_prefs
    while(fgets(line, sizeof(line), config_file) != NULL)
    {
        prefs = (volk_arch_pref_t *) realloc(prefs, (n_arch_prefs+1) * sizeof(*prefs));
        volk_arch_pref_t *p = prefs + n_arch_prefs;
        if(sscanf(line, "%s %s %s", p->name, p->impl_a, p->impl_u) == 3 && !strncmp(p->name, "volk_", 5))
        {
            n_arch_prefs++;
        }
    }
    fclose(config_file);
    *prefs_res = prefs;
    return n_arch_prefs;
}

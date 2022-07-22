#include <gnuradio/prefs.h>
#include <algorithm>

namespace fs = std::filesystem;

namespace gr {

prefs* prefs::get_instance()
{
    static prefs p;
    return &p;
}

YAML::Node prefs::get_section(const std::string& name)
{
    return get_instance()->_config[name];
}

const std::string prefs::get_string(const std::string& section,
                                    const std::string& option,
                                    const std::string& default_val)
{
    try {
        auto s = get_section(section);
        return s[option].as<std::string>();
    } catch (const std::exception& e) {
        return default_val;
    }
}

const double prefs::get_double(const std::string& section,
                               const std::string& option,
                               const double default_val)
{
    try {
        auto s = get_section(section);
        return s[option].as<double>();
    } catch (const std::exception& e) {
        return default_val;
    }
}

const long prefs::get_long(const std::string& section,
                           const std::string& option,
                           const long default_val)
{
    try {
        auto s = get_section(section);
        return s[option].as<long>();
    } catch (const std::exception& e) {
        return default_val;
    }
}

const bool prefs::get_bool(const std::string& section,
                           const std::string& option,
                           const bool default_val)
{
    try {
        auto s = get_section(section);
        return s[option].as<bool>();
    } catch (const std::exception& e) {
        return default_val;
    }
}

const char* prefs::appdata_path()
{
    const char* path;

    // first case, try HOME environment variable (unix)
    path = getenv("HOME");
    if (path)
        return path;

    // second case, try APPDATA environment variable (windows)
    path = getenv("APPDATA");
    if (path)
        return path;

    // fall-through case, nothing worked
    return tmp_path();
}

prefs::prefs()
{
    std::vector<std::string> fnames;

    // // find the preferences yaml file
    // // Find if there is a ~/.gnuradio/config.conf file
    // fs::path userconf = fs::path(userconf_path()) / "config.yml";
    // if (fs::exists(userconf)) {
    //     // fnames.push_back(userconf.string());
    //     _config = YAML::LoadFile(userconf.string());
    // }

    _read_files(_sys_prefs_filenames());
}

void prefs::_read_files(const std::vector<std::string>& filenames)
{
    for (const auto& fname : filenames) {
        if (!fs::exists(fname)) {
            std::cerr << "WARNING: Config file '" << fname
                      << "' could not be found for reading." << std::endl;
            continue;
        }

        try {
            auto tmp_node = YAML::LoadFile(fname);

            for (YAML::const_iterator it = tmp_node.begin(); it != tmp_node.end(); ++it) {
                std::string key = it->first.as<std::string>(); // <- key
                _config[key] = it->second;
            }

        } catch (std::exception& e) {
            std::cerr << "WARNING: Config file '" << fname
                      << "' failed to parse:" << std::endl;
            std::cerr << e.what() << std::endl;
            std::cerr << "Skipping it" << std::endl;
        }
    }
}

const char* prefs::tmp_path()
{
    const char* path;

    // first case, try TMP environment variable
    path = getenv("TMP");
    if (path)
        return path;

// second case, try P_tmpdir when its defined
#ifdef P_tmpdir
    if (P_tmpdir)
        return P_tmpdir;
#endif /*P_tmpdir*/

    // fall-through case, nothing worked
    return "/tmp";
}

std::vector<std::string> prefs::_sys_prefs_filenames()
{
    std::vector<std::string> fnames;

    fs::path dir = gr::prefsdir();
    if (fs::is_directory(dir)) {
        for (const auto& p : fs::directory_iterator(dir)) {
            // if (p.path().extension() == ".conf.yml")
            fnames.push_back(p.path().string());
        }
        std::sort(fnames.begin(), fnames.end());
    }

    // Find if there is a ~/.gnuradio/config.conf.yml file and add this to
    // the end of the file list to override any preferences in the
    // installed path config files.
    fs::path userconf = fs::path(gr::userconf_path()) / "config.yml";
    if (fs::exists(userconf)) {
        fnames.push_back(userconf.string());
    }

    return fnames;
}
} // namespace gr
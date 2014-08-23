/* -*- c++ -*- */

namespace gr {
  %rename(prefix) prefix;
  %rename(sysconfdir) sysconfdir;
  %rename(prefsdir) prefsdir;
  %rename(build_date) build_date;
  %rename(version) version;
  %rename(version_info) version_info;

  const std::string prefix();
  const std::string sysconfdir();
  const std::string prefsdir();
  const std::string build_date();
  const std::string version();
  const std::string major_version();
  const std::string api_version();
  const std::string minor_version();

}

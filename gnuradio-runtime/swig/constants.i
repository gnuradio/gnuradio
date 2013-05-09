/* -*- c++ -*- */

namespace gr {
  %rename(prefix) prefix;
  %rename(sysconfdir) sysconfdir;
  %rename(prefsdir) prefsdir;
  %rename(build_date) build_date;
  %rename(version) version;

  const std::string prefix();
  const std::string sysconfdir();
  const std::string prefsdir();
  const std::string build_date();
  const std::string version();
}

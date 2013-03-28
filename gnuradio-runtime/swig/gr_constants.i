/* -*- c++ -*- */

%rename(prefix) gr_prefix;
%rename(sysconfdir) gr_sysconfdir;
%rename(prefsdir) gr_prefsdir;
%rename(build_date) gr_build_date;
%rename(version) gr_version;

const std::string gr_prefix();
const std::string gr_sysconfdir();
const std::string gr_prefsdir();
const std::string gr_build_date();
const std::string gr_version();

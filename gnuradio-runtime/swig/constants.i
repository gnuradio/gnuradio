/* -*- c++ -*- */

%rename(prefix) gr::prefix;
%rename(sysconfdir) gr::sysconfdir;
%rename(prefsdir) gr::prefsdir;
%rename(build_date) gr::build_date;
%rename(version) gr::version;

const std::string gr::prefix();
const std::string gr::sysconfdir();
const std::string gr::prefsdir();
const std::string gr::build_date();
const std::string gr::version();

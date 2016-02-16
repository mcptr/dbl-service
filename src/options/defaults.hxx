#ifndef DBL_OPTIONS_DEFAULTS_HXX
#define DBL_OPTIONS_DEFAULTS_HXX

#include <string>

namespace dbl {
namespace defaults {

const int service_port = 7654;
const int http_responder_status_code = 404;
const std::string http_responder_status_text = "Not found";
const std::string network_ip4address = "127.0.0.1";
const std::string network_ip6address = "::1";

#ifdef __linux

const std::string basedir = "/etc/dnsblocker";
const std::string dbdir = "/var/db/dnsblocker";
const std::string logdir = "/var/log/dnsblocker";
const std::string rundir = "/var/run/dnsblocker";
const std::string sysconfdir = basedir;

const std::string network_interface = "lo";

const std::string dns_proxy = "unbound";
const std::string dns_proxy_config_dir = sysconfdir + "/dns-proxy";
const std::string dns_proxy_logfile = logdir + "/dbsblocker-proxy.log";
const std::string dns_proxy_pidfile = rundir + "/dbsblocker-proxy.pid";
const std::string dns_proxy_user = dns_proxy;

const std::string service_db = dbdir + "/dbsblocker.db";
const std::string service_config = sysconfdir + "/dnsblocker.conf";
const std::string service_pidfile = rundir + "/dbsblocker.pid";
const std::string service_scriptdir = sysconfdir + "/scripts/linux";
const std::string service_sysconfdir = sysconfdir;

#endif

} // defaults
} // dbl

#endif

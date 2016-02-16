#ifndef CONFIG_UNIX_HXX
#define CONFIG_UNIX_HXX

#include "base.hxx"
#include <string>

namespace dbl {

class UnixConfig : public BaseConfig
{
public:
	std::string platform = "unix";

	std::string base_dir = "/etc/dnsblocker";
	std::string db_dir = "/var/db/dnsblocker";
	std::string log_file = "/var/log/dnsblocker/dnsblocker.log";
	std::string templates_dir = "templates";

	std::string network_interface = "lo";

	std::string dns_proxy = "unbound";
	std::string dns_proxy_executable = "unbound";
	std::string dns_proxy_logfile = "dbsblocker-proxy.log";
	std::string dns_proxy_pidfile = "dbsblocker-proxy.pid";
	std::string dns_proxy_root_key = "root.key";
	std::string dns_proxy_user = "unbound";
	std::string dns_proxy_workdir = base_dir;

	std::string service_db = "dbsblocker.db";
	std::string service_config = "dnsblocker.conf";
	std::string service_pidfile = "dbsblocker.pid";
};

} // dbl

#endif

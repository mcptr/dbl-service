#include "unix.hxx"

namespace dbl {

UnixConfig::UnixConfig()
{
	platform = "unix";

	base_dir = "/etc/dnsblocker";
	db_dir = "/var/db/dnsblocker";
	log_file = "/var/log/dnsblocker/dnsblocker.log";
	templates_dir = base_dir + "/templates";

	network_interface = "lo";

	dns_proxy = "unbound";
	dns_proxy_config_dest_dir = "/var/run/dnsblocker";
	dns_proxy_executable = "unbound";
	dns_proxy_logfile = "/var/log/dnsblocker/proxy.log";
	dns_proxy_pidfile = "/var/run/dnsblocker/proxy.pid";
	dns_proxy_root_dir = "/";
	dns_proxy_root_key = base_dir + "/dnssec/root.key";
	dns_proxy_user = "unbound";
	dns_proxy_workdir = base_dir;

	service_db = "/var/db/dnsblocker/dbsblocker.db";
	service_config = base_dir + "/dnsblocker.conf";
	service_pidfile = "/var/run/dnsblocker/service.pid";
}

} // dbl
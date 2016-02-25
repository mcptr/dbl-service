#ifndef DBL_CONFIG_CONFIG_HXX
#define DBL_CONFIG_CONFIG_HXX

#include <string>
#include <vector>

namespace dbl {
namespace config {

class Config
{
public:
	bool is_debug = false;
	bool is_fatal = false;
	bool is_foreground = false;
	bool is_test = false;
	bool is_verbose = false;

	std::string platform;
	std::string base_dir;
	std::string db_dir;
	std::string log_file;
	std::string templates_dir;
	std::string script_dir;

	std::string logger_config_path = "log.conf";
	int service_port = 7654;

	bool http_responder_enable = true;
	short http_responder_port = 80;
	int http_responder_status_code = 404;
	std::string http_responder_status_text = "Not found";

	std::string network_ip4address  = "127.0.0.1";
	std::string network_ip6address = "::1";
	std::string network_interface = "";
	bool network_no_ip4 = false;
	bool network_no_ip6 = false;

	bool no_system_dns_proxy = false;
	bool no_update = false;

	std::string dns_proxy = "unbound";
	std::string dns_proxy_chroot = "";
	std::string dns_proxy_config = "";
	std::string dns_proxy_config_dest_dir = "";
	bool dns_proxy_disable_dnssec = false;
	std::string dns_proxy_executable = "unbound";
	std::string dns_proxy_include_dir = "";
	bool dns_proxy_generate_config = false;
	std::string dns_proxy_logfile = "dbsblocker-proxy.log";
	int dns_proxy_port = 53;
	std::string dns_proxy_pidfile = "dbsblocker-proxy.pid";
	std::string dns_proxy_root_key = "root.key";
	std::string dns_proxy_user = "unbound";
	std::string dns_proxy_workdir = base_dir;

	std::string service_db = "dbsblocker.db";
	std::string service_config = "dnsblocker.conf";
	std::string service_logfile = "dbsblocker.log";
	std::string service_pidfile = "dbsblocker.pid";
	std::string service_user = "dnsblocker";

	bool disable_list_update = false;

	std::vector<std::string> list_ids = {
		"CUSTOM"
	};
};

} // config
} // dbl

#endif

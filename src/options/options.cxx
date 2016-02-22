#include "options.hxx"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <sys/utsname.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace dbl {

void Options::parse(int argc, char** argv, BaseConfig& config)
{
	namespace fs = boost::filesystem;
	using std::string;
	po::options_description flags("Generic");
	po::options_description service("Service options");
	po::options_description dnsproxy("DNS proxy");
	po::options_description dnsproxy_custom("Custom DNS proxy instance options");
	po::options_description network("Network options");
	po::options_description http_responder("HTTP Responder");
	po::options_description lists("DNS lists options");
	po::options_description config_file_options("");

	string config_path;

	struct utsname utsn;
	int status = uname(&utsn);
	if(status != 0) {
		throw std::runtime_error("uname() failed");
	}

	string current_platform(utsn.sysname);
	boost::algorithm::to_lower(current_platform);
	string current_path = fs::current_path().string();

	bool no_config = false;

	flags.add_options()
		("help,h", "Display this help")
		("debug,D",
		 po::value(&(config.is_debug))->implicit_value(true)->zero_tokens()->default_value(config.is_debug),
		 "Debug mode"
		)
		("fatal,F",
		 po::value(&(config.is_fatal))->implicit_value(true)->zero_tokens()->default_value(config.is_fatal),
		 "Make all errors fatal"
		)
		("foreground,f",
		 po::value(&(config.is_foreground))->implicit_value(true)->zero_tokens()->default_value(config.is_foreground),
		 "Do not go into background"
		)
		("verbose,v",
		 po::value(&(config.is_verbose))->implicit_value(true)->zero_tokens()->default_value(config.is_verbose),
		 "Verbose run"
		)
		("test,T",
		 po::value(&(config.is_test))->implicit_value(true)->zero_tokens()->default_value(config.is_test),
		 "Do not start/generate anything - just test init"
		)
		;
	
	service.add_options()
		("basedir",
		 po::value(&(config.base_dir))->default_value(config.base_dir),
		 "base application directory"
		)
		("config,c",
		 po::value(&(config.service_config))->default_value(config.service_config),
		 "Configuration file path"
		)
		("no-config,N",
		 po::value(&no_config)->implicit_value(true)->zero_tokens()->default_value(no_config),
		 "Do not read configuration file"
		)
		("db",
		 po::value(&(config.service_db))->default_value(config.service_db),
		 "Database file path"
		)
		("logfile",
		 po::value(&(config.service_logfile))->default_value(config.service_logfile)
		)
		("logger-config-path",
		 po::value(&(config.logger_config_path))->default_value(
			 config.logger_config_path
		 )
		)
		("pidfile",
		 po::value(&(config.service_pidfile))->default_value(config.service_pidfile)
		)
		("platform",
		 po::value(&(config.platform))->default_value(current_platform),
		 "Override platform"
		)
		("service-port",
		 po::value(&(config.service_port))->default_value(config.service_port),
		 "Service port"
		)
		("script-dir",
		 po::value(&(config.script_dir))->default_value(config.script_dir),
		 "Location of scripts"
		)
		("service-user",
		 po::value(&(config.service_user))->default_value(config.service_user),
		 "Drop privileges and run as this user"
		)
		("templates-dir",
		 po::value(&(config.templates_dir))->default_value(config.templates_dir),
		 "Override platform (if guessed incorrect)"
		)
		("no-system-dns-proxy",
		 po::value(&(config.no_system_dns_proxy))->implicit_value(true)->zero_tokens()->default_value(false),
		 "Use system installed DNS Proxy server"
		)
		;
	lists.add_options()
		("disable-list-update",
		 po::value(&(config.disable_list_update))->implicit_value(true)->zero_tokens()->default_value(
			 config.disable_list_update
		 ),
		 "Do not update blocking lists"
		)
		;
	
	dnsproxy.add_options()
		("dns-proxy",
		 po::value(&(config.dns_proxy))->default_value(config.dns_proxy),
		 "DNS Proxy server"
		)
		("dns-proxy-include-dir",
		 po::value(&(config.dns_proxy_include_dir))->default_value(config.dns_proxy_include_dir)
		)	
	;
	dnsproxy_custom.add_options()
		("dns-proxy-chroot",
		 po::value(&(config.dns_proxy_chroot))->default_value(config.dns_proxy_chroot),
		 "DNS Proxy chroot dir (must be already set up)"
		)
		("dns-proxy-config",
		 po::value(&(config.dns_proxy_config))->default_value("")
		)
		("dns-proxy-config-destdir",
		 po::value(&(config.dns_proxy_config_dest_dir))->default_value(config.dns_proxy_config_dest_dir),
		 "Location of generated proxy configuration file"
		)
		("dns-proxy-disable-dnssec",
		 po::value(&(config.dns_proxy_disable_dnssec))->implicit_value(true)->zero_tokens()->default_value(
			 config.dns_proxy_disable_dnssec
		 )
		)
		("dns-proxy-generate-config",
		 po::value(&(config.dns_proxy_generate_config))->implicit_value(true)->zero_tokens()->default_value(
			 config.dns_proxy_generate_config
		 )
		)
		("dns-proxy-logfile",
		 po::value(&(config.dns_proxy_logfile))->default_value(config.dns_proxy_logfile)
		)
		("dns-proxy-pidfile",
		 po::value(&(config.dns_proxy_pidfile))->default_value(config.dns_proxy_pidfile)
		)
		("dns-proxy-port",
		 po::value(&(config.dns_proxy_port))->default_value(config.dns_proxy_port)
		)
		("dns-proxy-root-key",
		 po::value(&(config.dns_proxy_root_key))->default_value(config.dns_proxy_root_key)
		)
		("dns-proxy-user",
		 po::value(&(config.dns_proxy_user))->default_value(config.dns_proxy_user)
		)
		("dns-proxy-workdir",
		 po::value(&(config.dns_proxy_workdir))->default_value(config.dns_proxy_workdir)
		)
		;

	network.add_options()
		("network-interface",
		 po::value(&(config.network_interface))->default_value(config.network_interface),
		 "Network interface (defaults to loopback)"
		)
		("network-ip4address",
		 po::value(&(config.network_ip4address))->default_value(config.network_ip4address)
		)
		("network-ip6address",
		 po::value(&(config.network_ip6address))->default_value(config.network_ip6address)
		)
		("network-no-ip4",
		 po::value(&(config.network_no_ip4))->implicit_value(true)->zero_tokens()->default_value(
			 config.network_no_ip4
		 )
		)
		("network-no-ip6",
		 po::value(&(config.network_no_ip6))->implicit_value(true)->zero_tokens()->default_value(
			 config.network_no_ip6
		 )
		)
		;

	http_responder.add_options()
		("http-responder-enable",
		 po::value(&(config.http_responder_enable))->default_value(config.http_responder_enable),
		 "builtin HTTP responder"
		)
		("http-responder-port",
		 po::value(&(config.http_responder_port))->default_value(config.http_responder_port),
		 "Override builtin HTTP responder"
		)
		("http-responder-status-code",
		 po::value(&(config.http_responder_status_code))->default_value(config.http_responder_status_code)
		)
		("http-responder-status-text",
		 po::value(&(config.http_responder_status_text))->default_value(config.http_responder_status_text)
		)
		;

	all_.add(flags)
		.add(service)
		.add(network)
		.add(dnsproxy)
		.add(dnsproxy_custom)
		.add(http_responder)
		.add(lists);

	try {
		po::store(po::command_line_parser(argc, argv).options(all_).run(), vm_);
		po::notify(vm_);

		if(!has_help()) {
			if(config.is_debug) {
				std::cout << "Reading config file: " << config.service_config << std::endl;
			}
			
			if(!no_config) {
				if(!fs::exists(config.service_config)) {
					throw std::runtime_error(
						"Configuration file does not exist: " 
						+ config.service_config
					);
				}

				std::ifstream cf(config.service_config.c_str());

				if(!cf.good()) {
					throw std::runtime_error(
						"Cannot read configuration file" 
					);
				}

				po::store(po::parse_config_file(cf, all_, true), vm_);
				po::notify(vm_);
				cf.close();
			}
		}
		//throw_on_conflict();
	}
	catch(std::exception& e) {
		std::cerr << "Options error: " << e.what() << "\n";
		throw std::runtime_error(e.what());
	}
}

void Options::dump_variables_map() const
{
	for(const auto& it : vm_) {
		std::cout << ": "  << std::setw(48) << std::left << it.first;
		std::cout.width(0);
		auto& raw = it.second.value();
		std::string v;
		if (auto v = boost::any_cast<int>(&raw)) {
			std::cout << *v;
		}
		else if (auto v = boost::any_cast<std::string>(&raw)) {
			std::cout << *v;
		}
		else if (auto v = boost::any_cast<bool>(&raw)) {
			std::cout << *v;
		}

		std::cout << std::endl;
	}
}

bool Options::has_help() const
{
	return vm_.count("help");
}

void Options::display_help() const
{
	std::cout << all_ << std::endl;
}

void Options::throw_on_conflict(const std::string& x, const std::string& y)
{
	if(vm_.count(x) && !vm_[x].defaulted() &&
	   vm_.count(y) && !vm_[y].defaulted())
	{
		throw std::logic_error(
			std::string("Conflicting options '") +
			x + "' and '" + y + "'."
		);
	}
}

} // dbl

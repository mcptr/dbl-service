#include "options.hxx"
#include "defaults.hxx"

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <sys/utsname.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace dbl {

void Options::parse(int argc, char** argv)
{
	po::options_description flags("Generic");
	po::options_description service("Service options");
	po::options_description dnsproxy("DNS proxy options");
	po::options_description network("Network options");
	po::options_description http_responder("HTTP Responder");

	std::string config_path;
	std::string stropt;
	int intopt;

	struct utsname utsn;
	int status = uname(&utsn);
	if(status != 0) {
		throw std::runtime_error("uname() failed");
	}

	std::string current_platform(utsn.sysname);
	boost::algorithm::to_lower(current_platform);
	std::string current_path = boost::filesystem::current_path().string();

	flags.add_options()
		("help,h", "Display this help")
		("debug,D",
		 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
		 "Debug mode"
		)
		("fatal,F",
		 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
		 "Make all errors fatal"
		)
		("verbose,v",
		 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
		 "Verbose run"
		)
		;

	service.add_options()
		("basedir", po::value(&stropt)->default_value(defaults::basedir),
		 "base application directory"
		)
		("config,c",
		 po::value<std::string>(&config_path)->default_value(defaults::service_config),
		 "Configuration path"
		)
		("db",
		 po::value<std::string>(&stropt)->default_value(defaults::service_db),
		 "Database file path"
		)
		("foreground,f",
		 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
		 "Run in foreground"
		)
		("pidfile",
		 po::value(&stropt)->default_value(defaults::service_pidfile),
		 "Pidfile path"
		)
		("platform",
		 po::value(&stropt)->default_value(current_platform),
		 "Override platform (if guessed incorrect)"
		)
		("scriptdir", po::value(&stropt)->default_value(defaults::service_scriptdir),
		 "Directory containing scripts"
		)
		("sysconfdir",
		 po::value(&stropt)->default_value(defaults::sysconfdir),
		 "Base service configuration directory"
		)
		;

	dnsproxy.add_options()
		("dns-proxy",
		 po::value(&stropt)->default_value(defaults::dns_proxy),
		 "DNS Proxy service"
		)
		("dns-proxy-chroot",
		 po::value(&stropt)->default_value(""),
		 "DNS Proxy chroot dir (must be already set up)"
		)
		("dns-proxy-config",
		 po::value(&stropt)->default_value("")
		)
		("dns-proxy-config-dir",
		 po::value(&stropt)->default_value(defaults::dns_proxy_config_dir),
		 "DNS Proxy configuration dir"
		)
		("dns-proxy-executable",
		 po::value(&stropt)->default_value(""),
		 "DNS Proxy program path"
		)
		("dns-proxy-logfile",
		 po::value(&stropt)->default_value("")
		)
		("dns-proxy-pidfile",
		 po::value(&stropt)->default_value(defaults::dns_proxy_pidfile)
		)
		("dns-proxy-user",
		 po::value(&stropt)->default_value(defaults::dns_proxy_user),
		 "DNS Proxy working directory"
		)
		("dns-proxy-workdir",
		 po::value(&stropt)->default_value(defaults::dns_proxy_config_dir),
		 "DNS Proxy working directory"
		)
		;

	network.add_options()
		("network-interface",
		 po::value(&stropt)->default_value(defaults::network_interface),
		 "Network interface (defaults to loopback)"
		)
		("network-ip4address",
		 po::value(&stropt)->default_value(defaults::network_ip4address),
		 "IPv4 address"
		)
		("network-ip6address",
		 po::value(&stropt)->default_value(defaults::network_ip6address),
		 "IPv6 address"
		)
		("network-no-ip4",
		 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
		 "Disable IPv4 support"
		)
		("network-no-ip6",
		 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
		 "Disable IPv6 support"
		)
		("network-service-port",
		 po::value(&intopt)->default_value(defaults::service_port),
		 "Service port"
		)
		;

	http_responder.add_options()
		("http-responder-enable",
		 po::value<bool>()->implicit_value(true)->zero_tokens()->default_value(false),
		 "builtin HTTP responder"
		)
		("http-responder-status-code",
		 po::value(&intopt)->default_value(404),
		 "HTTP responder: status code"
		)
		("http-responder-status-text",
		 po::value<std::string>(&stropt)->default_value("Not found"),
		 "HTTP responder: status text"
		)
		;
	all_.add(flags).add(service).add(network).add(dnsproxy).add(http_responder);

	try {
		po::store(po::command_line_parser(argc, argv).options(all_).run(), vm_);
		po::notify(vm_);

		if(!has_help()) {
			// config_file_hidden.add(generic).add(server).add(builtins)
			// 	.add(infrastructure).add(resources)
			// 	.add(builtins_hidden).add(auth);
			
			// if(vm_["debug"].as<bool>()) {
			// 	std::cout << "Reading config file: " << config_path << std::endl;
			// }
			
			// ifstream cf(config_path.c_str());
			
			// if(cf.good()) {
			// 	po::store(po::parse_config_file(cf, config_file_hidden, true), vm_);
			// 	po::notify(vm_);
			// }
			// else {
			// 	if(!vm_["no_config"].as<bool>()) {
			// 		throw nix::InitializationError("Configuration file not found: " + config_path);
			// 	}
			// }
			
			// cf.close();
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

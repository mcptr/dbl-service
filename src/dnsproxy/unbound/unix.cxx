#include "unix.hxx"
#include <vector>
#include <boost/filesystem.hpp>


namespace dbl {

UnixUnbound::UnixUnbound(std::shared_ptr<RTApi> api)
	: Unbound(api)
{
}

std::string UnixUnbound::get_executable_name() const
{
	return "unbound";
}

void UnixUnbound::generate_config()
{
	namespace fs = boost::filesystem;

	fs::path path(
		api_->program_options.get<std::string>("dns-proxy-config-dir")
	);

	std::string user(
		api_->program_options.get<std::string>("dns-proxy-user")
	);

	std::string chroot(
		api_->program_options.get<std::string>("dns-proxy-chroot")
	);

	std::string logfile(
		api_->program_options.get<std::string>("dns-proxy-logfile")
	);

	std::string pidfile(
		api_->program_options.get<std::string>("dns-proxy-pidfile")
	);

	std::string ip4address(
		api_->program_options.get<std::string>("network-ip4address")
	);

	std::string ip6address(
		api_->program_options.get<std::string>("network-ip6address")
	);

	bool disable_ip_4(api_->program_options.get<bool>("network-no-ip4"));
	bool disable_ip_6(api_->program_options.get<bool>("network-no-ip6"));

	config_.push_back("directory: \"" + path.parent_path().string() + "\"");
	config_.push_back("pidfile: \"" + pidfile + "\"");
	config_.push_back("username: " + user);

	if(!chroot.empty()) {
		config_.push_back("chroot: \"" + chroot + "\"");
	}

	if(!logfile.empty()) {
		config_.push_back("logfile: \"" + logfile + "\"");
	}

	if(!disable_ip_4) {
		config_.push_back("interface: " + ip4address);
	}

	if(!disable_ip_6) {
		config_.push_back("interface: " + ip6address);
	}
}

} // dbl

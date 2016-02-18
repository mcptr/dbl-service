#include "unix.hxx"
#include "core/common.hxx"

#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <boost/filesystem.hpp>


namespace dbl {

UnixUnbound::UnixUnbound(std::shared_ptr<RTApi> api)
	: Unbound(api)
{
	pidfile_path_ = api_->config.dns_proxy_pidfile;
}

std::string UnixUnbound::get_executable_name() const
{
	return "unbound";
}

void UnixUnbound::start()
{
	if(!api_->config.no_system_dns_proxy) {
		std::vector<std::string> cmds = {
			"service unbound restart",
			"/etc/init.d/unbound restart",
			"/etc/rc.d/unbound restart",
			"/usr/local/etc/rc.d/unbound restart",
			"systemctl restart unbound",
		};

		bool success = false;
		for(auto const& cmd : cmds) {
			LOG(DEBUG) << "Trying: " << cmd;
			if(system(cmd.c_str()) == 0) {
				success = true;
				break;
			}
		}

		if(!success) {
			throw std::runtime_error(
				"Unable to start 'unbound' system service"
			);
		}

		std::string msg(
			"\nSome init scripts report success even\n"
			"when the service fails to start.\n"
			"You may need to restart 'unbound' service yourself to be sure.\n"
		);

		// TODO: check effective uid before logging info?
		LOG(INFO) << msg << std::endl;
	}
	else {
		std::string cmd = this->find_executable();
		if(cmd.empty()) {
			throw std::runtime_error("Unable to find dns proxy executable");
		}

		cmd.append(" -c " + config_file_path_);
		LOG(DEBUG) << cmd << std::endl;
		LOG(DEBUG) << "DNS Proxy config file: " << config_file_path_ << std::endl;
		if(system(cmd.c_str()) != 0) {
			throw std::runtime_error("Unable to start dns proxy. Command: " + cmd);
		}
	}
}

} // dbl

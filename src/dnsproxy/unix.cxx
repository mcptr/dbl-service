#include "unix.hxx"

namespace dbl {

Unix::Unix(std::shared_ptr<RTApi> api)
	: DNSProxy(api)
{
	pidfile_path_ = api_->config.dns_proxy_pidfile;
	pidof_ = dbl::find_executable("pidof");
}

std::string Unix::find_proxy_executable() const
{

	std::string executable = api_->config.dns_proxy_executable;

	if(executable.empty()) {
		executable = dbl::find_executable(this->get_executable_name());
	}

	if(executable.empty()) {
		throw std::runtime_error("Unable to locate dns proxy software");
	}

	LOG(INFO) << "DNS proxy program: " << executable;
	return executable;
}

} // dbl

#include "base.hxx"
#include "util/fs.hxx"


namespace dbl {

DNSProxy::DNSProxy(std::shared_ptr<RTApi> api)
	: api_(api)
{
}

std::string DNSProxy::find_executable() const
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

void DNSProxy::add_domain(const std::string& name)
{
	domains_.insert(name);
}

void DNSProxy::remove_domain(const std::string& name)
{
	domains_.erase(name);
}

std::size_t DNSProxy::count_domains()
{
	return domains_.size();
}

} // dbl

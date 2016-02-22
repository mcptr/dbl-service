#include "base.hxx"
#include "util/fs.hxx"


namespace dbl {

DNSProxy::DNSProxy(std::shared_ptr<RTApi> api)
	: api_(api)
{
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

void DNSProxy::set_value(const std::string& k,
						 const std::string& v)
{
	config_[k] = v;
}

void DNSProxy::generate_config()
{
	namespace fs = boost::filesystem;
	auto const& config = api_->config;

	if(!config.dns_proxy_workdir.empty()) {
		config_["DIRECTORY"] = config.dns_proxy_workdir;
	}
	else {
		config_["DIRECTORY"] = config.base_dir;
	}

	//config_["PIDFILE"] =  pidfile_path_;
	config_["USER"] = config.dns_proxy_user;
	config_["CHROOT"] = config.dns_proxy_chroot;
	config_["LOGFILE"] = config.dns_proxy_logfile;
	config_["PORT"] = std::to_string(config.dns_proxy_port);
}
} // dbl

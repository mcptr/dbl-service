#include "base.hxx"

namespace dbl {
namespace service {

Configurator::Configurator(const std::shared_ptr<RTApi> api)
	: api_(api)
{
}

void Configurator::configure(DNSProxy& proxy)
{
	ip4address_ = api_->config.network_ip4address;
	ip6address_ = api_->config.network_ip6address;
	interface_ = this->get_default_interface();

	if(ip4address_.compare("127.0.0.1") == 0
	   || ip6address_.compare("::1") == 0)
	{
		LOG(INFO) << "Using localhost address. "
				  << "Will default to interface: " << interface_;
	}
	else {
		interface_ = api_->config.network_interface;
		if(interface_.empty()) {
			throw std::runtime_error(
				"Create a dedicated interface to use with address: " +
				ip4address_
			);
		}
	}

	run_network_discovery();

	if(!available_interfaces_.count(interface_)) {
		throw std::runtime_error("Invalid interface: " + interface_);
	}

	if(!available_interfaces_[interface_].count(ip4address_)) {
		LOG(INFO) << "Configuring interface: " << interface_;
		this->configure_interface();
	}
	else {
		LOG(INFO) << "Interface already configured: "
				  << interface_ << " / " << ip4address_;
	}

	this->configure_dns_proxy(proxy);
}

void Configurator::configure_dns_proxy(DNSProxy& /* proxy */)
{
}

} // service
} // dbl

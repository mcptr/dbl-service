#include "factory.hxx"
#include "unbound.hxx"
#include "dnsmasq.hxx"

#include <boost/algorithm/string.hpp>
#include <stdexcept>

namespace dbl {
namespace dnsproxy {

std::unique_ptr<DNSProxy> create(std::shared_ptr<core::Api> api)
{
	std::string name = api->config.dns_proxy;
	boost::algorithm::to_lower(name);
	std::unique_ptr<DNSProxy> ptr;
	if(name.compare("dnsmasq") == 0) {
		ptr.reset(new DNSMasq(api));
	}
	else if(name.compare("unbound") == 0) {
		ptr.reset(new Unbound(api));
	}
	else {
		throw std::runtime_error("No support for: " + name);
	}

	return ptr;
}

} // dnsproxy
} // dbl

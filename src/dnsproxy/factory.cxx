#include "factory.hxx"
#include "unbound/unbound.hxx"

#include <boost/algorithm/string.hpp>
#include <stdexcept>

#if defined(__linux) || defined(__unix)
#include "unbound/unix.hxx"
typedef dbl::UnixUnbound ProxyImplementation_t;
#endif

namespace dbl {

std::unique_ptr<DNSProxy> create_dns_proxy(std::shared_ptr<RTApi> api)
{
	std::string name = api->config.dns_proxy;
	boost::algorithm::to_lower(name);
	std::unique_ptr<DNSProxy> ptr;
	if(name.compare("dnsmasq") == 0) {
		throw std::runtime_error("dnsmasq not implemented yet");
	}
	else if(name.compare("unbound") == 0) {
		
		ptr.reset(new ProxyImplementation_t(api));
	}
	else {
		throw std::runtime_error("No support for: " + name);
	}

	return ptr;
}

} // dbl

#ifndef DBL_SERVICE_CONFIGURATOR_HXX
#define DBL_SERVICE_CONFIGURATOR_HXX

#include "dbl/core/api.hxx"
#include "dbl/dnsproxy/dnsproxy.hxx"

#include <string>
#include <memory>
#include <set>
#include <unordered_map>


namespace dbl {
namespace service {
namespace configurator {

class Configurator
{
public:
	typedef std::set<std::string> AddressList_t;
	typedef std::unordered_map<std::string, AddressList_t> InterfaceList_t;

	Configurator() = delete;
	Configurator(const std::shared_ptr<core::Api> api);
	virtual ~Configurator() = default;

	virtual void configure(dnsproxy::DNSProxy& proxy);

	virtual std::string find_proxy_executable() const = 0;
	virtual std::string get_proxy_executable_name() const = 0;

protected:
	const std::shared_ptr<core::Api> api_;

	InterfaceList_t available_interfaces_;
	std::string interface_;
	std::string ip4address_;
	std::string ip6address_;
	std::string dns_proxy_executable_;


	virtual void run_network_discovery() = 0;
	virtual std::string get_default_interface() const = 0;
	virtual void configure_interface() = 0;
	virtual void configure_dns_proxy(dnsproxy::DNSProxy& proxy);
};

} // configurator
} // service
} // dbl

#endif

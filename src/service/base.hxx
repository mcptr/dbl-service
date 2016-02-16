#ifndef DBL_SERVICE_BASE_HXX
#define DBL_SERVICE_BASE_HXX

#include "core/rtapi.hxx"
#include "dnsproxy/base.hxx"

#include <string>
#include <set>
#include <unordered_map>
#include <memory>

namespace dbl {

class BaseService
{
public:
	typedef std::set<std::string> AddressList_t;
	typedef std::unordered_map<std::string, AddressList_t> InterfaceList_t;

	BaseService() = delete;
	explicit BaseService(std::shared_ptr<dbl::RTApi> api);
	virtual ~BaseService() = default;


	virtual void configure();
	virtual void start() final;
	virtual void stop() final;

protected:
	std::shared_ptr<dbl::RTApi> api_;
	InterfaceList_t available_interfaces_;
	std::string interface_;
	std::string ip4address_;
	std::string dns_proxy_executable_;

	std::unique_ptr<DNSProxy> dns_proxy_;

	virtual void run_network_discovery() = 0;
	virtual std::string get_default_interface() const = 0;

	virtual void configure_interface() = 0;
	virtual void configure_dns_proxy();

	virtual void start_dns_proxy() = 0;
	virtual void stop_dns_proxy() = 0;
	virtual void start_http_responder() = 0;
	//virtual bool stop_http_responder() = 0;

	virtual void flush_dns() = 0;
};

} // dbl

#endif

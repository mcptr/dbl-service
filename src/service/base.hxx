#ifndef DBL_SERVICE_BASE_HXX
#define DBL_SERVICE_BASE_HXX

#include "core/rtapi.hxx"
#include "dnsproxy/base.hxx"

#include "server/server.hxx"
#include "server/http_responder_connection.hxx"
#include "server/service_connection.hxx"

#include <string>
#include <set>
#include <unordered_map>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>


namespace dbl {

class BaseService
{
public:
	typedef std::set<std::string> AddressList_t;
	typedef std::unordered_map<std::string, AddressList_t> InterfaceList_t;

	BaseService() = delete;
	explicit BaseService(std::shared_ptr<dbl::RTApi> api);
	virtual ~BaseService() = default;

	static std::unique_ptr<BaseService> service_ptr;
	static std::mutex service_mtx_;

	virtual void configure();
	virtual void start() = 0;
	virtual void stop() = 0;

	virtual bool is_already_running() = 0;

protected:
	std::shared_ptr<dbl::RTApi> api_;
	std::vector<std::thread> threads_;

	InterfaceList_t available_interfaces_;
	std::string interface_;
	std::string ip4address_;
	std::string dns_proxy_executable_;

	std::unique_ptr<DNSProxy> dns_proxy_;

	virtual void serve() final;

	virtual void run_network_discovery() = 0;
	virtual std::string get_default_interface() const = 0;

	virtual void configure_interface() = 0;
	virtual void configure_dns_proxy();

	virtual void start_dns_proxy() = 0;
	virtual void stop_dns_proxy() = 0;
	virtual void flush_dns() = 0;

	virtual void start_service();

	std::unique_ptr<
		service::Server<service::ServiceConnection>
		> server_ptr_;

	std::unique_ptr<
		service::Server<service::HTTPResponderConnection>
		> http_responder_ptr_;
};

} // dbl

#endif

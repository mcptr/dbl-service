#ifndef DBL_SERVICE_SERVICE_HXX
#define DBL_SERVICE_SERVICE_HXX

#include "server/server.hxx"
#include "server/http_responder_connection.hxx"
#include "server/service_connection.hxx"
#include "configurator/configurator.hxx"

#include "dbl/core/api.hxx"
#include "dbl/dnsproxy/dnsproxy.hxx"

#include "dbl/updater/updater.hxx"

#include <string>
#include <set>
#include <unordered_map>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>


namespace dbl {
namespace service {

class Service
{
public:
	Service() = delete;
	explicit Service(std::shared_ptr<dbl::core::Api> api);
	virtual ~Service() = default;

	static std::unique_ptr<Service> service_ptr;

	virtual void configure();
	virtual void run() = 0;
	virtual void stop() = 0;

	virtual bool is_already_running() = 0;
	virtual void stop_service() final;

protected:
	std::shared_ptr<core::Api> api_;
	std::unique_ptr<dnsproxy::DNSProxy> dns_proxy_;
	std::unique_ptr<configurator::Configurator> configurator_;

	std::vector<std::thread> threads_;
	std::mutex service_mtx_;
	std::condition_variable service_cv_;

	bool is_updated_ = false;


	virtual void run_service() final;

	virtual void start_servers() final;
	virtual void stop_servers() final;

	virtual void drop_privileges() = 0;
	// virtual void start() = 0;
	// virtual void reload() = 0;

	virtual void start_dns_proxy() = 0;
	virtual void stop_dns_proxy() = 0;
	virtual void flush_dns() = 0;

	std::unique_ptr<
		server::Server<server::ServiceConnection>
		> server_ptr_;

	std::unique_ptr<
		server::Server<server::HTTPResponderConnection>
		> http_responder_ptr_;

	std::unique_ptr<updater::Updater> updater_ptr_;
};

} // service
} // dbl

#endif

#ifndef DBL_SERVICE_SERVICE_HXX
#define DBL_SERVICE_SERVICE_HXX

#include "worker/worker.hxx"
#include "server/server.hxx"
#include "server/http_responder_connection.hxx"
#include "server/service_connection.hxx"
#include "configurator/configurator.hxx"

#include "dbl/core/api.hxx"
#include "dbl/dnsproxy/dnsproxy.hxx"
#include "dbl/ipc/ipc.hxx"
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
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

namespace dbl {
namespace service {

class ServiceSHM
{
public:
	boost::interprocess::interprocess_semaphore sync_semaphore {0};
	boost::interprocess::interprocess_semaphore reloader_semaphore {0};
};

class Service
{
public:
	static bool signaled_exit;
	static std::unique_ptr<Service> service_ptr;

	Service() = delete;
	explicit Service(std::shared_ptr<dbl::core::Api> api);
	virtual ~Service();

	virtual void run() = 0;
	virtual void stop() = 0;

	virtual void signal_stop() final;

	virtual bool is_already_running() = 0;

	virtual void configure();

protected:
	std::shared_ptr<core::Api> api_;
	std::unique_ptr<dnsproxy::DNSProxy> dns_proxy_;
	std::unique_ptr<ipc::SharedMemory<ServiceSHM>> shm_ptr_;
	std::unique_ptr<configurator::Configurator> configurator_;

	std::unique_ptr<worker::Worker> worker_ptr_;
	std::condition_variable cv_;

	virtual void start_dns_proxy() = 0;
	virtual void stop_dns_proxy() = 0;
	virtual void flush_dns() = 0;

	virtual void run_worker();
};

} // service
} // dbl

#endif

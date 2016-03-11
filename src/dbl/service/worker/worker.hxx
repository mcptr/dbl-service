#ifndef DBL_SERVICE_WORKER_WORKER_HXX
#define DBL_SERVICE_WORKER_WORKER_HXX

#include "dbl/core/api.hxx"
#include "dbl/service/server/server.hxx"
#include "dbl/service/server/service_connection.hxx"
#include "dbl/service/server/http_responder_connection.hxx"
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <memory>

namespace dbl {
namespace service {
namespace worker {

class Worker
{
public:
	Worker() = delete;
	Worker(std::shared_ptr<dbl::core::Api> api);
	virtual ~Worker() = default;

	virtual void run(std::condition_variable& cv);
	virtual void stop();
	virtual void run_servers() final;
	virtual void stop_servers() final;

protected:
	std::shared_ptr<dbl::core::Api> api_;
	std::atomic<bool> stop_flag_ {false};
	std::mutex mtx_;

	std::vector<std::thread> threads_;

	std::unique_ptr<
		server::Server<server::ServiceConnection>
		> server_ptr_;

	std::unique_ptr<
		server::Server<server::HTTPResponderConnection>
		> http_responder_ptr_;

	virtual bool run_updater() final;

};

} // worker
} // service
} // dbl

#endif

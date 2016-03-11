#include "service.hxx"
#include "configurator/factory.hxx"
//#include "server/service_connection.hxx"

#include "dbl/core/common.hxx"
#include "dbl/dnsproxy/factory.hxx"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace dbl {
namespace service {

bool Service::signaled_exit = false;

std::unique_ptr<Service>
Service::service_ptr = nullptr;

Service::Service(std::shared_ptr<core::Api> api)
	: api_(api),
	  dns_proxy_(std::move(dnsproxy::create(api_)))
{
}

Service::~Service()
{
	shm_ptr_.reset();
}

void Service::configure()
{
	configurator_ = std::move(configurator::create(api_));
	configurator_->configure(*dns_proxy_);
	dns_proxy_->create_config();
}

void Service::run_worker()
{
	worker_ptr_.reset(new worker::Worker(api_));
	worker_ptr_->run(cv_);
}

void Service::signal_stop()
{
	worker_ptr_->stop();
	cv_.notify_all();
}

// void Service::stop_service()
// {
// 	if(api_->config.is_foreground) {
// 		this->stop_reloader_thread();
// 	}

// 	this->stop_servers();
// 	service_cv_.notify_all();
// }

// void Service::run_reloader_thread()
// {
// 	reloader_thread_ = std::thread(
// 		[this]() {
// 			using namespace boost::posix_time;
// 			bool reload_signaled = false;
// 			while(!this->reloader_stop_flag_) {
// 				ptime t = microsec_clock::universal_time();
// 				if(this->shm_ptr_->get_object()->reloader_semaphore.timed_wait(
// 					   t + milliseconds(300)))
// 				{
// 					if(!this->reloader_stop_flag_) {
// 						reload_signaled = true;
// 						break;
// 					}
// 				}
// 			}
			
// 			if(reload_signaled) {
// 				this->reload();
// 			}
// 			LOG(DEBUG) << "Reloader thread finished";
// 		}
// 	);
// }

// void Service::stop_reloader_thread()
// {
// 	LOG(DEBUG) << "Stopping reloader";
// 	reloader_stop_flag_ = true;
// 	this->shm_ptr_->get_object()->reloader_semaphore.post();
// 	LOG(DEBUG) << "Joining Reloader";
// 	reloader_thread_.join();
// 	LOG(DEBUG) << "Reloader stopped";
// }

} // service
} // dbl

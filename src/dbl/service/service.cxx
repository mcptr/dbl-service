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

} // service
} // dbl

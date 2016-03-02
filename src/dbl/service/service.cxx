#include "service.hxx"
#include "configurator/factory.hxx"
#include "server/service_connection.hxx"

#include "dbl/core/common.hxx"
#include "dbl/dnsproxy/factory.hxx"


namespace dbl {
namespace service {

std::unique_ptr<Service>
Service::service_ptr = nullptr;


Service::Service(std::shared_ptr<core::Api> api)
	: api_(api)
{
	dns_proxy_ = std::move(dnsproxy::create(api_));
}

void Service::configure()
{
	configurator_ = std::move(configurator::create(api_));
	configurator_->configure(*dns_proxy_);
	dns_proxy_->create_config();
}

void Service::run_service()
{
	this->start_servers();
	bool is_ok = true;

	if(api_->config.is_foreground) {
		LOG(WARNING) << "\n#############################################\n"
					 << "# WARNING: Running in foreground\n"
					 << "# without dropping privileges.\n"
					 << "#\n"
					 << "# Use SIGINT to quit\n"
					 << "#############################################\n";

		signal(SIGINT, [](int /*sig*/) {
				Service::service_ptr->stop_service();
			}
		);
	}
	else {
		LOG(DEBUG) << "Dropping privileges";
		try {
			this->drop_privileges();
		}
		catch(const std::runtime_error& e) {
			LOG(ERROR) << e.what();
			is_ok = false;
		}
	}

	if(is_ok) {
		std::unique_lock<std::mutex> lock(service_mtx_);
		service_cv_.wait(lock);
	}
}

void Service::start_servers()
{
	if(api_->config.service_port) {
		try {
			this->server_ptr_.reset(
				new server::Server<server::ServiceConnection>(
					this->api_,
					this->api_->config.service_port
				)
			);
		}
		catch(const std::exception& e) {
			LOG(ERROR) << "ERROR: Unable to start service server";
			LOG(ERROR) << e.what();
			LOG(ERROR) << "Aborting.";
			_exit(0);
		}

		if(this->server_ptr_) {
			threads_.push_back(
				std::thread([this]() {
						this->server_ptr_->run();
					}
				)
			);
		}
	}

	if(api_->config.http_responder_enable) {
		try {
			this->http_responder_ptr_.reset(
				new server::Server<server::HTTPResponderConnection>(
					this->api_,
					this->api_->config.http_responder_port
				)
			);
		}
		catch(const std::exception& e) {
			LOG(ERROR) << "ERROR: Unable to start HTTPResponder";
			LOG(ERROR) << e.what();
			LOG(ERROR) << "Aborting.";
			_exit(0);
		}

		if(this->http_responder_ptr_) {
			threads_.push_back(
				std::thread([this]() {
						http_responder_ptr_->run();
					}
				)
			);
		}
	}

	if(!api_->config.no_update) {
		updater_ptr_.reset(new updater::Updater(api_));

		updater_ptr_->enable_list_update(
			!api_->config.disable_list_update
		);

		threads_.push_back(
			std::thread([this]() {
					this->is_updated_ = updater_ptr_->run();
					if(this->is_updated_) {
						LOG(INFO) << "Service update ready. Restarting";
						this->stop_service();
					}
				}
			)
		);
	}
}

void Service::stop_servers()
{
	if(this->server_ptr_) {
		this->server_ptr_->stop();
	}

	if(this->http_responder_ptr_) {
		this->http_responder_ptr_->stop();
	}

	if(this->updater_ptr_) {
		this->updater_ptr_->stop();
	}

	LOG(DEBUG) << "Joining threads";
	for(auto& t : threads_) {
		t.join();
	}
	LOG(DEBUG) << "All threads joined";

	// Delete all instances to get rid of
	// the underlying sockets
	this->server_ptr_.reset();
	this->http_responder_ptr_.reset();
	this->updater_ptr_.reset();
}

void Service::stop_service()
{
	service_cv_.notify_all();
	this->stop_servers();
}

} // service
} // dbl

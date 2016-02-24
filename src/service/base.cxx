#include "base.hxx"
#include "core/common.hxx"
#include "dnsproxy/factory.hxx"
#include "configurator/factory.hxx"

#include "server/service_connection.hxx"

namespace dbl {

std::unique_ptr<BaseService>
BaseService::service_ptr = nullptr;


BaseService::BaseService(std::shared_ptr<dbl::RTApi> api)
	: api_(api)
{
	dns_proxy_ = std::move(create_dns_proxy(api_));
}

void BaseService::configure()
{
	configurator_ = std::move(service::create_configurator(api_));
	configurator_->configure(*dns_proxy_);
	dns_proxy_->create_config();
}

void BaseService::run_service()
{
	this->start_servers();
	if(api_->config.is_foreground) {
		LOG(WARNING) << "\n#############################################\n"
					 << "# WARNING: Running in foreground\n"
					 << "# without dropping privileges.\n"
					 << "#\n"
					 << "# Use SIGINT to quit\n"
					 << "#############################################\n";

		signal(SIGINT, [](int /*sig*/) {
				BaseService::service_ptr->stop_service();
			}
		);
	}
	else {
		this->drop_privileges();
	}

	std::unique_lock<std::mutex> lock(service_mtx_);
	service_cv_.wait(lock);
}

void BaseService::start_servers()
{
	if(api_->config.service_port) {
		try {
			this->server_ptr_.reset(
				new service::Server<service::ServiceConnection>(
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
				new service::Server<service::HTTPResponderConnection>(
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
		updater_ptr_.reset(new service::Updater(api_));

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

void BaseService::stop_servers()
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
}

void BaseService::stop_service()
{
	LOG(INFO) << "stop_service()";
	service_cv_.notify_all();
	this->stop_servers();
}

} // dbl

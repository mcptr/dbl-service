#include "base.hxx"
#include "core/common.hxx"
#include "dnsproxy/factory.hxx"
#include "util/fs.hxx"

#include "server/service_connection.hxx"

namespace dbl {

std::unique_ptr<BaseService>
BaseService::service_ptr = nullptr;

std::mutex BaseService::service_mtx_;

BaseService::BaseService(std::shared_ptr<dbl::RTApi> api)
	: api_(api)
{
	dns_proxy_ = std::move(create_dns_proxy(api_));
}

void BaseService::configure()
{
	ip4address_ = api_->config.network_ip4address;
	ip6address_ = api_->config.network_ip6address;
	interface_ = this->get_default_interface();

	if(ip4address_.compare("127.0.0.1") == 0
	   || ip6address_.compare("::1") == 0)
	{
		LOG(INFO) << "Using localhost address. "
				  << "Will default to interface: " << interface_;
	}
	else {
		interface_ = api_->config.network_interface;
		if(interface_.empty()) {
			throw std::runtime_error(
				"Create a dedicated interface to use with address: " +
				ip4address_
			);
		}
	}

	run_network_discovery();

	if(!available_interfaces_.count(interface_)) {
		throw std::runtime_error("Invalid interface: " + interface_);
	}

	if(!available_interfaces_[interface_].count(ip4address_)) {
		LOG(INFO) << "Configuring interface: " << interface_;
		this->configure_interface();
	}
	else {
		LOG(INFO) << "Interface already configured: "
				  << interface_ << " / " << ip4address_;
	}

	this->configure_dns_proxy();
}

void BaseService::configure_dns_proxy()
{
	dns_proxy_->create_config();
}

void BaseService::start_service()
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

	if(!api_->config.disable_list_update) {

	}
}

void BaseService::serve()
{
	while(!service_mtx_.try_lock()) {
		std::this_thread::sleep_for(
			std::chrono::milliseconds(100)
		);
	}

	if(this->server_ptr_) {
		this->server_ptr_->stop();
	}

	if(this->http_responder_ptr_) {
		this->http_responder_ptr_->stop();
	}

	LOG(INFO) << "Joining threads";
	for(auto& t : threads_) {
		t.join();
	}
}

} // dbl

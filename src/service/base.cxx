#include "base.hxx"
#include "core/common.hxx"
#include "dnsproxy/factory.hxx"
#include "util/fs.hxx"


namespace dbl {

BaseService::BaseService(std::shared_ptr<dbl::RTApi> api)
	: api_(api)
{
	dns_proxy_ = std::move(create_dns_proxy(api_));
}

void BaseService::configure()
{
	ip4address_ = api_->config.network_ip4address;
	interface_ = this->get_default_interface();

	if(ip4address_.compare("127.0.0.1") == 0) {
		LOG(INFO) << "Using IP4 address: 127.0.0.1. "
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

void BaseService::start()
{
	this->flush_dns();
	this->start_dns_proxy();
	
	if(api_->config.http_responder_enable) {
		this->start_http_responder();
	}
}

void BaseService::stop()
{
	this->stop_dns_proxy();
	this->flush_dns();
	//if(api_->program_options.get<bool>("http-responder-enable")) {
		//this->stop_http_responder();
	//}
}


} // dbl

#include "worker.hxx"
#include "dbl/updater/updater.hxx"

#include <chrono>

namespace dbl {
namespace service {
namespace worker {

Worker::Worker(std::shared_ptr<dbl::core::Api> api)
	: api_(api)
{
}

void Worker::run(std::condition_variable& cv)
{
	int min_intval = 3600;
	auto intval = std::chrono::seconds(
		api_->config.update_interval > min_intval
		? api_->config.update_interval
		: min_intval
	);

	bool updated = this->run_updater();
	if(!updated) {
		while(!stop_flag_) {
			bool quit = false;
			std::unique_lock<std::mutex> lock(mtx_);
			if(cv.wait_for(lock, intval) == std::cv_status::timeout) {
				LOG(DEBUG) << "Worker running";
				if(this->run_updater()) {
					LOG(DEBUG) << "Service update ready.";
					quit = true;
				}

				if(quit) {
					LOG(DEBUG) << "Worker run finished";
					break;
				}
			}
		}
	}

	LOG(DEBUG) << "Stopping servers";
	this->stop_servers();
}

void Worker::stop()
{
	stop_flag_ = true;
}
void Worker::run_servers()
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
}

void Worker::stop_servers()
{
	if(this->server_ptr_) {
		this->server_ptr_->stop();
	}

	if(this->http_responder_ptr_) {
		this->http_responder_ptr_->stop();
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
}

bool Worker::run_updater()
{
	bool status = false;
	if(!api_->config.no_update) {
		updater::Updater updater(api_);
		status = updater.run();
	}

	return status;
}

} // worker
} // service
} // dbl


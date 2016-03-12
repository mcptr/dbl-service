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

// void Service::run_service()
// {
// 	using namespace boost::posix_time;

// 	if(!api_->config.is_foreground) {
// 		ptime t = microsec_clock::universal_time();
// 		time_duration interval(milliseconds(3000));

// 		ServiceSHM* service_shm_ptr = this->shm_ptr_->get_object();
// 		if(service_shm_ptr == nullptr) {
// 			LOG(ERROR) << "Could not get service shm object";
// 			return;
// 		}

// 		LOG(DEBUG) << "Waiting for master process to get ready ";
// 		if(!service_shm_ptr->sync_semaphore.timed_wait(t + interval)) {
// 			LOG(ERROR) << "Parent not ready. Exiting";
// 			return;
// 		}
// 		else {
// 			LOG(DEBUG) << "Sync semaphore posted";
// 		}
// 	}

// 	this->run_reloader_thread();
// 	this->start_servers();
// 	bool is_ok = true;

// 	if(api_->config.is_foreground) {
// 		LOG(WARNING) << "\n#############################################\n"
// 					 << "# WARNING: Running in foreground\n"
// 					 << "# without dropping privileges.\n"
// 					 << "#\n"
// 					 << "# Use SIGINT to quit\n"
// 					 << "#############################################\n";

// 		signal(SIGINT, [](int /*sig*/) {
// 				Service::service_ptr->stop_service();
// 			}
// 		);
// 	}
// 	else {
// 		LOG(DEBUG) << "Dropping privileges";
// 		try {
// 			this->drop_privileges();
// 		}
// 		catch(const std::runtime_error& e) {
// 			LOG(ERROR) << e.what();
// 			is_ok = false;
// 		}
// 	}

// 	if(is_ok) {
// 		std::unique_lock<std::mutex> lock(service_mtx_);
// 		service_cv_.wait(lock);
// 	}
// }

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


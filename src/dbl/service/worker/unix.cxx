#include "unix.hxx"
#include <chrono>
#include <pwd.h>
#include <grp.h>


namespace dbl {
namespace service {
namespace worker {

Unix::Unix(std::shared_ptr<dbl::core::Api> api)
	: Worker(api)
{
}


void Unix::run(std::condition_variable& cv)
{
	this->run_servers();
	this->drop_privileges();
	this->Worker::run(cv);
}

void Unix::drop_privileges()
{
	struct passwd* p = getpwnam(api_->config.service_user.c_str());

	if(p == nullptr) {
		throw std::runtime_error(
			"Invalid user: " + api_->config.service_user
		);
	}

	uid_t user_id = p->pw_uid;
	gid_t group_id = p->pw_gid;

	if(user_id == 0 || group_id == 0) {
		throw std::runtime_error(
			"Not an unprivileged user: " +
			api_->config.service_user
		);
	}

	if(setgroups(0, nullptr) != 0) {
		PLOG(WARNING) << "setgroups()";
	}
		
	if(setgid(group_id) != 0) {
		PLOG(ERROR) << "setgid(" << group_id << ")";
		throw std::runtime_error(
			"Cannot drop privileges (setgid() failed)"
		);
	}

	if(setegid(group_id) != 0) {
		PLOG(ERROR) << "setegid(" << group_id << ")";
		throw std::runtime_error(
			"Cannot drop privileges (setegid() failed)"
		);
	}

	if(setuid(user_id) != 0) {
		PLOG(ERROR) << "setuid(" << user_id << ")";
		throw std::runtime_error(
			"Cannot drop privileges (setuid() failed)"
		);
	}

	if(seteuid(user_id) != 0) {
		PLOG(ERROR) << "seteuid(" << user_id << ")";
		throw std::runtime_error(
			"Cannot drop privileges (seteuid() failed)"
		);
	}

	if(setuid(0) == 0) {
		throw std::runtime_error("Dropping privileges failed");
	}

	if(setgid(0) == 0) {
		throw std::runtime_error("Dropping privileges failed");
	}
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


} // worker
} // service
} // dbl


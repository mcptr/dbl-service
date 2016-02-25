#include "updater.hxx"

namespace dbl {
namespace service {

Updater::Updater(std::shared_ptr<core::Api> api)
	: api_(api)
{
}

void Updater::enable_list_update(bool state)
{
	enable_list_update_ = state;
}

void Updater::stop()
{
	stop_flag_ = true;
	cv_.notify_all();
}


bool Updater::run()
{
	while(!stop_flag_) {
		std::unique_lock<std::mutex> lock(mtx_);

		std::cv_status status = 
			cv_.wait_for(lock, std::chrono::seconds(1));

		if(status == std::cv_status::timeout) {
			if(this->update()) {
				return true;
			}
		}
		else {
			LOG(INFO) << "Updater exiting.";
			break;
		}
	}

	return false;
}

bool Updater::update()
{
	LOG(INFO) << "Running update";

	return false;
}

} // service
} // dbl

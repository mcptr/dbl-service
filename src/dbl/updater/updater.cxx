#include "updater.hxx"
#include "dbl/core/constants.hxx"
#include "dbl/manager/domain_list_manager.hxx"
#include "dbl/types/types.hxx"

#include <algorithm>

namespace dbl {
namespace updater {

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
	for(auto const& it : api_->config.list_ids) {
		LOG(DEBUG) << "UPDATER LIST: " << it;
	}

	LOG(INFO) << "Running initial update";
	update();

	while(!is_updated_ && !stop_flag_) {
		std::unique_lock<std::mutex> lock(mtx_);

		std::cv_status status = 
			cv_.wait_for(lock, std::chrono::seconds(3600 * 4));

		if(status == std::cv_status::timeout) {
			update();
		}
		else {
			LOG(INFO) << "Updater exiting.";
			break;
		}
	}

	return is_updated_;
}

void Updater::update()
{
	LOG(INFO) << "Running update";
	update_lists();
}

void Updater::update_lists()
{
	manager::DomainListManager mgr(api_);
	auto lists = mgr.get();

	auto const& ids = api_->config.list_ids;
	for(auto& lst : *lists) {
		// if(lst.custom) {
		// 	continue;
		// }
		bool do_update = ids.empty();
		if(!ids.empty()) {
			do_update = std::find(
				ids.begin(), ids.end(), lst.name) != ids.end();
		}

		if(do_update) {
			net::HTTPRequest rq;
			std::string url(core::constants::UPDATE_URL);
			url.append("/" + lst.name + ".json");
			if(rq.fetch(url, 200)) {
				//2DO: check last modif tstamp
				try {
					lst.from_json(rq.get_result());
					LOG(INFO) << "Updating domain list: " << lst.name;
					mgr.import(lst, false);
					//is_updated_ = true;
				}
				catch(const std::runtime_error& e) {
					LOG(ERROR) << e.what();
				}
			}
		}
	}
}

} // updater
} // dbl

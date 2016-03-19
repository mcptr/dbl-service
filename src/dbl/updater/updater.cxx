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

bool Updater::run()
{
	is_updated_ = false;

	update_lists();

	return is_updated_;
}


void Updater::update_lists()
{
	if(api_->config.disable_list_update) {
		return;
	}

	manager::DomainListManager mgr(api_);
	auto lists = mgr.get();

	auto const& ids = api_->config.list_ids;
	for(auto& lst : *lists) {
		if(lst.custom) {
			//continue;
		}

		bool do_update = ids.empty();
		if(!ids.empty()) {
			do_update = std::find(
				ids.begin(), ids.end(), lst.name) != ids.end();
		}

		if(do_update) {
			std::string url;
			if(!lst.url.empty()) {
				url = lst.url;
			}
			else {
				url = core::constants::UPDATE_URL;
				url.append("/" + lst.name + ".json");
			}
			net::http::Request rq(url);
			rq.set_if_modified_since(lst.mtime);
			auto response_ptr = rq.fetch();

			long last_modified = (
				response_ptr ? response_ptr->get_last_modified() : 0
			);

			if(!last_modified || (lst.mtime < last_modified)) {
				LOG(INFO) << "Updating domain list: " << lst.name;
				auto ptr = rq.fetch();
				if(ptr) {
					try {
						lst.from_json(ptr->get_data());
						mgr.import(lst, false);
						is_updated_ = true;
					}
					catch(const std::runtime_error& e) {
						LOG(ERROR) << e.what();
					}
				}
			}
		}
	}
}

} // updater
} // dbl

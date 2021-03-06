#include "dbl/core/common.hxx"

#include "dbl/init/init.hxx"
#include "dbl/mgmt/mgmt.hxx"
#include "dbl/options/options.hxx"
#include "dbl/service/service.hxx"
#include "dbl/query/query.hxx"

#include "dbl/manager/domain_list_manager.hxx"
#include "dbl/manager/domain_manager.hxx"

#include <memory>
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <curl/curl.h>


#if defined(__unix)
#include "dbl/config/unix.hxx"
#include "dbl/service/unix.hxx"
typedef dbl::service::Unix ServiceImplementation_t;
typedef dbl::config::Unix ConfigImplementation_t;
#endif


INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv)
{
	namespace fs = boost::filesystem;
	using namespace dbl;
	using dbl::service::Service;

	ConfigImplementation_t config;
	Options po;

	const std::string bin_abs_path = fs::canonical(argv[0]).string();

	curl_global_init(CURL_GLOBAL_DEFAULT);

	try {
		po.parse(argc, argv, config);
		if(po.get<bool>("debug")) {
			po.dump_variables_map();
		}

		init::validate_options(po);
	}
	catch(std::exception& e) {
		std::cerr << "Invalid command " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	if(po.has_help()) {
		po.display_help();
		return 0;
	}

	while(!Service::signaled_exit) {
		init::setup_logging(po);

		std::shared_ptr<core::Api> api;
		std::shared_ptr<db::DB> db;

		try {
			if(!fs::exists(config.service_db)) {
				fs::path db_path(config.service_db);
				LOG(INFO) << "Creating database directory "
						  << db_path.parent_path().string();
				fs::create_directories(db_path.parent_path());
			}

			db.reset(new db::DB(config.service_db, 5));
			db->init();

			api.reset(new core::Api(config, db));

			if(po.get<bool>("query")) {
				query::Query q(po, api);
				bool success = q.run();
				return (success ? EXIT_SUCCESS : EXIT_FAILURE);
			}
			else { // management
				mgmt::Mgmt manager(api, po);
				if(manager.has_work()) {
					bool ok = manager.manage();
					return (ok ? EXIT_SUCCESS : EXIT_FAILURE);
				}
			}

			if(!config.is_test) {
				LOG(INFO) << std::endl;
				LOG(INFO) << "###### Starting...";
			}

			service::Service::service_ptr.reset(
				new ServiceImplementation_t(api)
			);

			if(service::Service::service_ptr->is_already_running()) {
				std::string msg("Another instance already running");
				std::cerr << msg << std::endl;
				LOG(ERROR) << msg;
				return EXIT_FAILURE;
			}

			service::Service::service_ptr->configure();
		}
		catch(const fs::filesystem_error& e) {
			std::string msg(e.code().message());
			msg.append(": ");
			msg.append(e.path1().string());
			std::cerr << msg <<  std::endl;
			LOG(ERROR) << e.what();
			return EXIT_FAILURE;
		}
		catch(const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
			LOG(ERROR) << e.what();
			LOG(ERROR) << "Aborting";
			if(service::Service::service_ptr) {
				service::Service::service_ptr->stop();
			}
			return EXIT_FAILURE;
		}

		if(!config.is_test) {
			dbl::manager::DomainListManager dlmgr(api);
			dbl::manager::DomainManager dmgr(api);
			api->status.init();
			api->status.total_lists = dlmgr.count();
			api->status.total_blocked_domains = dmgr.count_blocked();

			service::Service::service_ptr->run();
		}

		api.reset();
		db.reset();
		service::Service::service_ptr.reset();

		if(Service::signaled_exit) {
			LOG(INFO) << "##### Stopped.";
		}
		else {
			LOG(INFO) << "##### Stopped. Reloading.";
		}
	}

	curl_global_cleanup();

	return EXIT_SUCCESS;
}

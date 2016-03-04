#include "dbl/core/common.hxx"

#include "dbl/manager/domain_manager.hxx"
#include "dbl/manager/domain_list_manager.hxx"
#include "dbl/options/options.hxx"
#include "dbl/service/service.hxx"
#include "dbl/query/query.hxx"

#include <cstdlib>
#include <memory>
#include <string>
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

void setup_logging(const dbl::Options& po);

void manage_domains(std::shared_ptr<dbl::core::Api> api,
					std::vector<std::string> block,
					std::vector<std::string> unblock);

void manage_import_export(std::shared_ptr<dbl::core::Api> api,
						  std::vector<std::string> lst);


INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv)
{
	namespace fs = boost::filesystem;

	ConfigImplementation_t config;
	dbl::Options po;
	try {
		po.parse(argc, argv, config);
		if(po.has_help()) {
			po.display_help();
			return 0;
		}
		if(po.get<bool>("debug")) {
			po.dump_variables_map();
		}

		setup_logging(po);
	}
	catch(std::exception& e) {
		std::cerr << "Invalid command " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	std::string proxy = po.get<std::string>("dns-proxy");
	boost::algorithm::to_lower(proxy);
	
	if(proxy.compare("unbound") != 0 && proxy.compare("dnsmasq") != 0) {
		std::cerr << "Unsupported dns proxy software: " << proxy
				  << "\nYou must use 'unbound' or 'dnsmasq'.\n"
				  << std::endl;
		return EXIT_FAILURE;
	}

	std::shared_ptr<dbl::core::Api> api;
	std::shared_ptr<dbl::db::DB> db;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	try {
		if(!fs::exists(config.service_db)) {
			fs::path db_path(config.service_db);
			LOG(INFO) << "Creating database directory "
					  << db_path.parent_path().string();
			fs::create_directories(db_path.parent_path());
		}

		db.reset(new dbl::db::DB(config.service_db, 5));
		db->init();

		api.reset(new dbl::core::Api(config, db));
		
		if(po.get<bool>("query")) {
			dbl::query::Query q(po, api);
			bool success = q.run();
			return (success ? EXIT_SUCCESS : EXIT_FAILURE);
		}
		else { // management
			auto const export_lists =
				po.get<std::vector<std::string>>("export-lists");

			auto const block_domains =
				po.get<std::vector<std::string>>("block");

			auto const unblock_domains =
				po.get<std::vector<std::string>>("unblock");

			bool is_management = (
				!export_lists.empty() ||
				!block_domains.empty() ||
				!unblock_domains.empty()
			);

			manage_domains(api, block_domains, unblock_domains);
			manage_import_export(api, export_lists);

			if(is_management) {
				return EXIT_SUCCESS;
			}
		}

		if(!config.is_test) {
			LOG(INFO) << std::endl;
			LOG(INFO) << "Starting...";
			LOG(INFO) << "################################################";
		}

		dbl::service::Service::service_ptr.reset(
			new ServiceImplementation_t(api)
		);

		if(dbl::service::Service::service_ptr->is_already_running()) {
			std::string msg("Another instance already running");
			std::cerr << msg << std::endl;
			LOG(ERROR) << msg;
			return EXIT_FAILURE;
		}

		dbl::service::Service::service_ptr->configure();
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
		std::cerr << "Exception in main(): " << e.what() << std::endl;
		LOG(ERROR) << e.what();
		LOG(ERROR) << "Aborting";
		if(dbl::service::Service::service_ptr) {
			dbl::service::Service::service_ptr->stop();
		}
		return EXIT_FAILURE;
	}

	if(!config.is_test) {
		dbl::service::Service::service_ptr->run();
	}

	dbl::service::Service::service_ptr.reset();
	api.reset();
	db.reset();

	curl_global_cleanup();

	LOG(DEBUG) << "main() exit success";
	return EXIT_SUCCESS;
}

void setup_logging(const dbl::Options& po)
{
	bool is_foreground = po.get<bool>("foreground");
	bool is_debug = po.get<bool>("debug");
	std::string logfile = po.get<std::string>("logfile");
	std::string path = po.get<std::string>("logger-config-path");
	el::Configurations conf(path);

	if(!logfile.empty()) {
		conf.setGlobally(el::ConfigurationType::Filename, logfile);
	}

	conf.setGlobally(
		el::ConfigurationType::ToStandardOutput,
		is_foreground ? "true" : "false");

	conf.set(
		el::Level::Debug,
		el::ConfigurationType::Enabled,
		is_debug ? "true" : "false");

	el::Loggers::addFlag(el::LoggingFlag::AutoSpacing);
	el::Loggers::addFlag(el::LoggingFlag::ImmediateFlush);
	el::Loggers::setDefaultConfigurations(conf, true);
	el::Loggers::reconfigureAllLoggers(conf);
	//el::Helpers::setCrashHandler(crash_handler);
}

void manage_domains(std::shared_ptr<dbl::core::Api> api,
					std::vector<std::string> block,
					std::vector<std::string> unblock)
{	
	dbl::manager::DomainManager mgr(api);

	if(!unblock.empty()) {
		mgr.unblock_domains(unblock);
	}

	if(!block.empty()) {
		for(auto const& domain : block) {
			// slow...
			bool found = std::find(
				unblock.begin(),
				unblock.end(),
				domain
			) != unblock.end();

			if(found) {
				std::string msg(
					"Conflicting domain (will be blocked): "
				);
				msg.append(domain);
				if(api->config.is_fatal) {
					throw std::runtime_error(msg);
				}
				std::cerr << "WARNING: " << msg 
						  << std::endl;
				LOG(WARNING) << msg;
			}
		}

		mgr.block_domains(block);
	}
}

void manage_import_export(std::shared_ptr<dbl::core::Api> api,
						  std::vector<std::string> lst)
{
	if(lst.size()) {
		dbl::manager::DomainListManager mgr(api);
		for(auto const& name : lst) {
			auto dl = mgr.get(name, true);
			if(dl) {
				std::cout << "\nList: " << name << std::endl;
				std::cout << dl->to_json_string() << std::endl;
			}
		}
	}
}

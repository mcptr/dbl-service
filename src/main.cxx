#include "core/common.hxx"

#include "options/options.hxx"
#include "status/status.hxx"
#include "service/service.hxx"

#if defined(__unix)
#include "config/unix.hxx"
#include "service/unix.hxx"
typedef dbl::service::Unix ServiceImplementation_t;
typedef dbl::config::Unix ConfigImplementation_t;
#elif defined(_WIN32)
//...
#endif

#include <cstdlib>
#include <memory>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>


void setup_logging(const dbl::Options& po)
{
	bool is_foreground = po.get<bool>("foreground");
	std::string logfile = po.get<std::string>("logfile");
	std::string path = po.get<std::string>("logger-config-path");
	el::Configurations conf(path);


	conf.setGlobally(
		el::ConfigurationType::ToStandardOutput,
		is_foreground ? "true" : "false");

	if(!logfile.empty()) {
		conf.setGlobally(el::ConfigurationType::Filename, logfile);
	}

	el::Loggers::setDefaultConfigurations(conf, true);
	el::Loggers::reconfigureAllLoggers(conf);
	//el::Helpers::setCrashHandler(crash_handler);
}


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

	if(!config.is_test) {
		LOG(INFO) << std::endl;
		LOG(INFO) << "Starting...";
		LOG(INFO) << "################################################";
	}

	std::shared_ptr<dbl::core::Api> api;
	std::shared_ptr<dbl::db::DB> db;

	try {
		if(!fs::exists(config.service_db)) {
			fs::path db_path(config.service_db);
			LOG(INFO) << "Creating database directory "
					  << db_path.parent_path().string();
			fs::create_directories(db_path.parent_path());
		}

		db.reset(new dbl::db::DB(config.service_db, 5));
		api.reset(new dbl::core::Api(config, db));
		
		// dbl::Status status(api);
		// status.print_lists();
		// status.print_domains();
		
		dbl::service::Service::service_ptr.reset(
			new ServiceImplementation_t(api)
		);

		if(dbl::service::Service::service_ptr->is_already_running()) {
			throw std::runtime_error("Another instance already running");
		}

		db->init();

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
		std::cerr << e.what() << std::endl;
		LOG(ERROR) << e.what();
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

	LOG(DEBUG) << "main() exit success";
	return EXIT_SUCCESS;
}

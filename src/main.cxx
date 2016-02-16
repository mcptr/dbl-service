#include "core/common.hxx"
INITIALIZE_EASYLOGGINGPP

#include "options/options.hxx"
#include "status/status.hxx"

#if defined(__linux)
#include "service/linux.hxx"
typedef dbl::LinuxService ServiceImplementation_t;
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
	el::Configurations conf;

	conf.setGlobally(
		el::ConfigurationType::ToStandardOutput,
		is_foreground ? "true" : "false");

	//conf.setGlobally(el::ConfigurationType::Filename, log_path);

	el::Loggers::setDefaultConfigurations(conf, true);
	el::Loggers::reconfigureAllLoggers(conf);
	//el::Helpers::setCrashHandler(crash_handler);
}


int main(int argc, char** argv)
{
	dbl::Options po;
	try {
		po.parse(argc, argv);
		if(po.has_help()) {
			po.display_help();
			return 0;
		}
		if(po.get<bool>("debug")) {
			po.dump_variables_map();
		}
	}
	catch(std::exception& e) {
		std::cerr << "Invalid command " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	setup_logging(po);

	std::string proxy = po.get<std::string>("dns-proxy");
	boost::algorithm::to_lower(proxy);
	
	if(proxy.compare("unbound") != 0 && proxy.compare("dnsmasq") != 0) {
		std::cerr << "Unsupported dns proxy software: " << proxy
				  << "\nYou must use 'unbound' or 'dnsmasq'.\n"
				  << std::endl;
		return EXIT_FAILURE;
	}

	std::unique_ptr<ServiceImplementation_t> service_ptr;

	try {
		std::shared_ptr<dbl::RTApi> rtapi(new dbl::RTApi(po));
		rtapi->db->init();
		
		dbl::Status status(rtapi);
		status.print_lists();
		status.print_domains();
		
		service_ptr.reset(new ServiceImplementation_t(rtapi));

		service_ptr->configure();
	}
	catch(const boost::filesystem::filesystem_error& e) {
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
		return EXIT_FAILURE;
	}

	service_ptr->start();

	return EXIT_SUCCESS;
}

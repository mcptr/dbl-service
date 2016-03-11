#include "init.hxx"

#include <string>
#include <boost/algorithm/string.hpp>

namespace dbl {
namespace init {

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
	LOG(DEBUG) << "Log configuration file:" << path;
	//el::Helpers::setCrashHandler(crash_handler);
}

void validate_options(const dbl::Options& po)
{
	std::string proxy = po.get<std::string>("dns-proxy");
	boost::algorithm::to_lower(proxy);
	
	if(proxy.compare("unbound") != 0 && proxy.compare("dnsmasq") != 0) {
		throw std::runtime_error("Invalid dnsproxy software");
	}
}

} // init
} // dbl

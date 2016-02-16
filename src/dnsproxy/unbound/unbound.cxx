#include "unbound.hxx"

#include <boost/filesystem.hpp>

namespace dbl {

Unbound::Unbound(std::shared_ptr<RTApi> api)
	: DNSProxy(api)
{
}

std::string Unbound::get_executable_name() const
{
	return "unbound";
}

void Unbound::create_config()
{
	namespace fs = boost::filesystem;

	fs::path path(
		api_->program_options.get<std::string>("dns-proxy-config")
	);

	if(path.empty()) {
		path = api_->program_options.get<std::string>("dns-proxy-config-dir");
		path /= "proxy-unbound.conf";
	}
	
	fs::create_directories(path.parent_path());
	this->generate_config();
	LOG(INFO) << "server: ";
	for(auto const& entry : config_) {
		LOG(INFO) << "    " << entry;
	}

}

} // dbl

#include "unbound.hxx"

#include <boost/filesystem.hpp>

namespace dbl {

Unbound::Unbound(std::shared_ptr<RTApi> api)
	: DNSProxy(api)
{
}

void Unbound::create_config() const
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
	LOG(INFO) << "SAVE  CONFIG: " << path.string() << "\n"
			  << path.parent_path().string();
}

} // dbl

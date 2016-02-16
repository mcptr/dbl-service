#include "unbound.hxx"
#include "template/template.hxx"

#include <boost/filesystem.hpp>
#include <fstream>
#include <ctime>


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
	using std::string;
	using std::ofstream;

	auto const& opts = api_->program_options;
	fs::path path(opts.get<string>("dns-proxy-config"));

	if(path.empty()) {
		path = opts.get<string>("basedir");
		path /= "proxy-unbound.conf";
	}
	
	if(!fs::exists(path)) {
		LOG(WARNING) << "Configuration file does not exist. "
					 << "Creating directory: " << path.parent_path();
		fs::create_directories(path.parent_path());
	}

	config_file_path_ = path.string();

	bool gen_config(opts.get<bool>("dns-proxy-generate-config"));

	if(gen_config) {
		Template tpl;
		tpl.load(opts.get<string>("templates-dir"), "unbound.conf");

		LOG(INFO) << "Generating unbound config: " << path.string();

		string ws = "    ";

		time_t t(time(nullptr));
		config_["GENERATION_TIME"] = std::asctime(localtime(&t));

		this->generate_config();

		ofstream ofh(config_file_path_);
		ofh << tpl.render(config_) << std::endl;

		bool no_ip4 = opts.get<bool>("network-no-ip4");
		bool no_ip6 = opts.get<bool>("network-no-ip6");
		std::string ip4address(opts.get<string>("network-ip4address"));
		std::string ip6address(opts.get<string>("network-ip4address"));

		ofh << ws
			<< (no_ip4 ? "do-ip4: no" : "interface: " + ip4address)
			<< std::endl;

		ofh << ws
			<< (no_ip6 ? "do-ip6: no" : "interface: " + ip6address)
			<< std::endl;

		for(auto const& domain : domains_) {
			ofh << ws << "local-data: \"" << domain
			   << " IN A " << ip4address
			   << "\"" << std::endl;
		}

		auto ptr = std::move(api_->db->get_domains());
		for(auto const& domain : *ptr) {
			ofh << ws << "local-data: \"" << domain.name
			   << " IN A " << ip4address
			   << "\"" << std::endl;
		}

		ofh.close();
	}

}

void Unbound::generate_config()
{
	namespace fs = boost::filesystem;
	auto const& options = api_->program_options;

	fs::path path(options.get<std::string>("basedir"));
		
	config_["DIRECTORY"] = path.parent_path().string();
	config_["PIDFILE"] =  pidfile_path_;
	config_["USER"] = options.get<std::string>("dns-proxy-user");
	config_["CHROOT"] = options.get<std::string>("dns-proxy-chroot");
	config_["LOGFILE"] = options.get<std::string>("dns-proxy-logfile");
	config_["PORT"] = std::to_string(options.get<int>("dns-proxy-port"));
}


} // dbl

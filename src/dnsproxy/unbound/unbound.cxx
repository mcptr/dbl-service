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

	auto const& config = api_->config;
	fs::path path(config.dns_proxy_config);

	if(!path.empty()) {
		if(!fs::exists(path)) {
			std::string msg("Configuration file does not exist: ");
			msg.append(path.string());
			LOG(ERROR) << msg;
			throw std::runtime_error(msg);
		}
	}

	if(config.dns_proxy_generate_config) {
		path = config.dns_proxy_config_dest_dir;
		path /= "proxy-unbound.conf";

		config_file_path_ = path.string();

		Template tpl;
		tpl.load(config.templates_dir, "unbound.conf");

		LOG(INFO) << "Generating unbound config: " << path.string();

		string ws = "    ";

		time_t t(time(nullptr));
		config_["GENERATION_TIME"] = std::asctime(localtime(&t));

		this->generate_config();

		ofstream ofh(config_file_path_);
		ofh << tpl.render(config_) << std::endl;

		ofh << ws
			<< (config.network_no_ip4
				? "do-ip4: no"
				: "interface: " + config.network_ip4address)
			<< std::endl;

		ofh << ws
			<< (config.network_no_ip6
				? "do-ip6: no"
				: "interface: " + config.network_ip6address)
			<< std::endl;

		if(!config.dns_proxy_disable_dnssec) {
			ofh << ws 
				<< "auto-trust-anchor-file: \""
				<< config.dns_proxy_root_key
				<< "\""
				<< std::endl;
		}


		for(auto const& domain : domains_) {
			ofh << ws << "local-data: \"" << domain
			   << " IN A " << config.network_ip4address
			   << "\"" << std::endl;
		}

		auto ptr = std::move(api_->db()->get_domains());
		for(auto const& domain : *ptr) {
			ofh << ws << "local-data: \"" << domain.name
			   << " IN A " << config.network_ip4address
			   << "\"" << std::endl;
		}

		ofh.close();
	}

}

void Unbound::generate_config()
{
	namespace fs = boost::filesystem;
	auto const& config = api_->config;

	fs::path path(config.base_dir);

	config_["DIRECTORY"] = (
		(!config.dns_proxy_root_dir.empty())
		? config.dns_proxy_root_dir
		: path.string()
	);
	config_["PIDFILE"] =  pidfile_path_;
	config_["USER"] = config.dns_proxy_user;
	config_["CHROOT"] = config.dns_proxy_chroot;
	config_["LOGFILE"] = config.dns_proxy_logfile;
	config_["PORT"] = std::to_string(config.dns_proxy_port);
}


} // dbl

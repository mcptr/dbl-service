#include "unbound.hxx"
#include "template/template.hxx"

#include <boost/filesystem.hpp>
#include <set>
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

		time_t t(time(nullptr));
		config_["GENERATION_TIME"] = std::asctime(localtime(&t));

		this->generate_config();

		ofstream ofh(config_file_path_);
		ofh << tpl.render(config_) << std::endl;

		if(config.network_no_ip4) {
			ofh << ws_ << "do-ip4: no" << std::endl;
		}
		else {
			ofh << ws_ << "interface: " << config.network_ip4address
				<< std::endl;
		}

		if(config.network_no_ip6) {
			ofh << ws_ << "do-ip6: no" << std::endl;
		}
		else {
			ofh << ws_ << "interface: " << config.network_ip6address
				<< std::endl;
		}

		if(!config.dns_proxy_disable_dnssec) {
			ofh << ws_ 
				<< "auto-trust-anchor-file: \""
				<< config.dns_proxy_root_key
				<< "\""
				<< std::endl;
		}

		ofh << std::endl;

		for(auto const& domain : domains_) {
			if(!config.network_no_ip4) {
				save_domain(ofh, IPv4, domain, config.network_ip4address);
			}

			if(!config.network_no_ip6) {
				save_domain(ofh, IPv6, domain, config.network_ip6address);
			}
		}

		auto ptr = std::move(api_->db()->get_domains());
		for(auto const& domain : *ptr) {
			if(!config.network_no_ip4) {
				save_domain(ofh, IPv4, domain.name, config.network_ip4address);
			}

			if(!config.network_no_ip6) {
				save_domain(ofh, IPv6, domain.name, config.network_ip6address);
			}
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

void Unbound::save_domain(std::ofstream& fh,
						  Protocol_t proto,
						  const std::string& domain,
						  const std::string& address) const
{
	fh << ws_ << "local-data: \"" << domain
	   << " IN " <<	(proto == IPv4 ? "A" : "AAAA")
	   << " " << address
	   << "\"" << std::endl;
}

} // dbl

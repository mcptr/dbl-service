#include "unbound.hxx"
#include "dbl/manager/domain_manager.hxx"
#include "dbl/template/template.hxx"

#include <boost/filesystem.hpp>
#include <set>
#include <ctime>


namespace dbl {
namespace dnsproxy {

Unbound::Unbound(std::shared_ptr<core::Api> api)
	: DNSProxy(api)
{
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
		time_t t(time(nullptr));
		config_["GENERATION_TIME"] = std::asctime(localtime(&t));

		if(!config.no_system_dns_proxy) {
			path = config.dns_proxy_include_dir;
			if(!fs::is_directory(path)) {
				throw std::runtime_error(
					"Invalid system dns proxy include dir: " + path.string()
				);
			}
			path /= "dnsblocker.conf";
			config_file_path_ = path.string();
		
		}
		else {
			path = config.dns_proxy_config_dest_dir;
			path /= "proxy-unbound.conf";
			config_file_path_ = path.string();
		}

		LOG(INFO) << "Generating unbound config: " << path.string();

		ofstream ofh(config_file_path_);
		if(config.no_system_dns_proxy) {
			this->generate_config();

			Template tpl;
			tpl.load(config.templates_dir, "unbound.conf");

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
		}
		else {
			ofh << "server:" << std::endl;
		}

		this->generate_domains_config(ofh);

		ofh.close();
	}

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

void Unbound::generate_domains_config(std::ofstream& ofh) const
{
	for(auto const& domain : domains_) {
		if(!api_->config.network_no_ip4) {
			this->save_domain(
				ofh, IPv4, domain, api_->config.network_ip4address
			);
		}

		if(!api_->config.network_no_ip6) {
			this->save_domain(
				ofh, IPv6, domain, api_->config.network_ip6address
			);
		}
	}

	manager::DomainManager domain_mgr(api_);
	auto ptr = std::move(domain_mgr.get());
	for(auto const& domain : *ptr) {
		if(!api_->config.network_no_ip4) {
			this->save_domain(
				ofh, IPv4, domain.name, api_->config.network_ip4address
			);
		}

		if(!api_->config.network_no_ip6) {
			this->save_domain(
				ofh, IPv6, domain.name, api_->config.network_ip6address
			);
		}
	}
}

} // dnsproxy
} // dbl

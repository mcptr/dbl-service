#ifndef DBL_DNSPROXY_DNSMASQ_HXX
#define DBL_DNSPROXY_DNSMASQ_HXX

#include "dnsproxy.hxx"

#include <fstream>

namespace dbl {
namespace dnsproxy {

class DNSMasq : public DNSProxy
{
public:
	DNSMasq() = delete;
	DNSMasq(std::shared_ptr<core::Api> api);
	virtual ~DNSMasq() = default;

	void create_config();

protected:
	virtual void save_domain(
		std::ofstream& fh,
		const std::string& domain,
		const std::string& address) const;

	virtual void generate_domains_config(std::ofstream& off) const final;
};

} // dnsproxy
} // dbl

#endif

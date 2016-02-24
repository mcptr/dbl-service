#ifndef DBL_DNSPROXY_DNSMASQ_HXX
#define DBL_DNSPROXY_DNSMASQ_HXX

#include "dnsproxy/base.hxx"

#include <fstream>

namespace dbl {

class DNSMasq : public DNSProxy
{
public:
	typedef enum { IPv4, IPv6 } Protocol_t;

	DNSMAsq() = delete;
	DNSMasq(std::shared_ptr<RTApi> api);
	virtual ~DNSMasq() = default;

	void create_config();
	virtual std::string get_executable_name() const = 0;

protected:
	const std::string ws_ = "    ";

	virtual void generate_config();

	virtual void save_domain(
		std::ofstream& fh,
		Protocol_t proto,
		const std::string& domain,
		const std::string& address) const;

	virtual void generate_domains_config(std::ofstream& off) const final;
};

} // dbl


#endif

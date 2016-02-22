#ifndef DBL_DNSPROXY_UNBOUND_HXX
#define DBL_DNSPROXY_UNBOUND_HXX

#include "dnsproxy/base.hxx"

#include <fstream>

namespace dbl {

class Unbound : public DNSProxy
{
public:
	typedef enum { IPv4, IPv6 } Protocol_t;

	Unbound() = delete;
	Unbound(std::shared_ptr<RTApi> api);
	virtual ~Unbound() = default;

	void create_config();

protected:
	const std::string ws_ = "    ";

	virtual void save_domain(
		std::ofstream& fh,
		Protocol_t proto,
		const std::string& domain,
		const std::string& address) const;

	virtual void generate_domains_config(std::ofstream& off) const final;
};

} // dbl


#endif

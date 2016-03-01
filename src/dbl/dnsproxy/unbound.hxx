#ifndef DBL_DNSPROXY_UNBOUND_HXX
#define DBL_DNSPROXY_UNBOUND_HXX

#include "dnsproxy.hxx"

#include <fstream>

namespace dbl {
namespace dnsproxy {

class Unbound : public DNSProxy
{
public:
	Unbound() = delete;
	Unbound(std::shared_ptr<core::Api> api);
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

} // dnsproxy
} // dbl


#endif

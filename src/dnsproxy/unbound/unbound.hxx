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
	virtual std::string get_executable_name() const = 0;

protected:
	const std::string ws_ = "    ";

	virtual void generate_config();

	virtual void save_domain(
		std::ofstream& fh,
		Protocol_t proto,
		const std::string& domain,
		const std::string& address) const;
};

} // dbl


#endif

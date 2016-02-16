#ifndef DBL_DNSPROXY_UNBOUND_HXX
#define DBL_DNSPROXY_UNBOUND_HXX

#include "dnsproxy/base.hxx"

namespace dbl {

class Unbound : public DNSProxy
{
public:
	Unbound() = delete;
	Unbound(std::shared_ptr<RTApi> api);
	virtual ~Unbound() = default;

	void create_config();
	virtual std::string get_executable_name() const = 0;

protected:
	virtual void generate_config();
};

} // dbl


#endif

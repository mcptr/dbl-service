#ifndef DBL_SERVICE_CONFIGURATOR_UNIX_HXX
#define DBL_SERVICE_CONFIGURATOR_UNIX_HXX

#include "configurator.hxx"
#include "dbl/core/api.hxx"

#include <memory>
#include <string>

namespace dbl {
namespace service {
namespace configurator {

class Unix : public Configurator
{
public:
	Unix() = delete;
	Unix(const std::shared_ptr<core::Api> api);
	virtual ~Unix() = default;

	virtual std::string find_proxy_executable() const;
	virtual std::string get_proxy_executable_name() const;

protected:
	std::string dns_proxy_pidfile_path_;

	virtual void run_network_discovery();
	virtual std::string get_default_interface() const;
	virtual void configure_interface();
	virtual void configure_dns_proxy(dnsproxy::DNSProxy& proxy);
};

} // configurator
} // service
} // dbl

#endif

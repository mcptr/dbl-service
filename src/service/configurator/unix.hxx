#ifndef DBL_SERVICE_CONFIGURATOR_UNIX_HXX
#define DBL_SERVICE_CONFIGURATOR_UNIX_HXX

#include "base.hxx"
#include "core/rtapi.hxx"

#include <memory>
#include <string>

namespace dbl {
namespace service {

class UnixConfigurator : public Configurator
{
public:
	UnixConfigurator() = delete;
	UnixConfigurator(const std::shared_ptr<RTApi> api);
	virtual ~UnixConfigurator() = default;

	virtual std::string find_proxy_executable() const;
	virtual std::string get_proxy_executable_name() const;

protected:
	std::string dns_proxy_pidfile_path_;

	virtual void run_network_discovery();
	virtual std::string get_default_interface() const;
	virtual void configure_interface();
	virtual void configure_dns_proxy(DNSProxy& proxy);
};

} // service
} // dbl

#endif

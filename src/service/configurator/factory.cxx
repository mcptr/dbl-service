#include "base.hxx"

#if defined(__unix)
#include "unix.hxx"
typedef dbl::service::UnixConfigurator Configurator_t;
#endif

namespace dbl {
namespace service {

std::unique_ptr<Configurator> create_configurator(std::shared_ptr<RTApi> api)
{
	std::unique_ptr<Configurator> ptr(
		new Configurator_t(api)
	);

	return std::move(ptr);
}

} // service
} // dbl

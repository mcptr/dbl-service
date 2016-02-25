#include "factory.hxx"

#if defined(__unix)
#include "unix.hxx"
typedef dbl::service::configurator::Unix Configurator_t;
#endif

namespace dbl {
namespace service {
namespace configurator {

std::unique_ptr<Configurator> create(std::shared_ptr<core::Api> api)
{
	std::unique_ptr<Configurator> ptr(
		new Configurator_t(api)
	);

	return std::move(ptr);
}

} // configurator
} // service
} // dbl

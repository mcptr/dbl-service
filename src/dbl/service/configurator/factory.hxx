#ifndef DBL_SERVICE_CONFIGURATOR_FACTORY_HXX
#define DBL_SERVICE_CONFIGURATOR_FACTORY_HXX

#include "configurator.hxx"
#include "dbl/core/api.hxx"

#include <memory>


namespace dbl {
namespace service {
namespace configurator {

std::unique_ptr<Configurator> create(std::shared_ptr<core::Api> api);

} // namespace
} // service
} // dbl

#endif

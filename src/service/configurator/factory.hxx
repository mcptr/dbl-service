#ifndef DBL_SERVICE_CONFIGURATOR_FACTORY_HXX
#define DBL_SERVICE_CONFIGURATOR_FACTORY_HXX

#include "base.hxx"
#include "core/rtapi.hxx"

#include <memory>


namespace dbl {
namespace service {

std::unique_ptr<Configurator> create_configurator(std::shared_ptr<RTApi> api);

} // service
} // dbl

#endif

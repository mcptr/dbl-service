#ifndef DBL_DNSPROXY_FACTORY_HXX
#define DBL_DNSPROXY_FACTORY_HXX

#include "dnsproxy.hxx"
#include "dbl/core/api.hxx"

#include <memory>


namespace dbl {
namespace dnsproxy {

std::unique_ptr<DNSProxy> create(std::shared_ptr<core::Api> api);

} // dnsproxy
} // dbl

#endif

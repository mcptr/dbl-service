#ifndef DBL_DNSPROXY_FACTORY_HXX
#define DBL_DNSPROXY_FACTORY_HXX

#include "base.hxx"
#include "core/rtapi.hxx"

#include <memory>


namespace dbl {

std::unique_ptr<DNSProxy> create_dns_proxy(std::shared_ptr<RTApi> api);

} // dbl

#endif

#ifndef DBL_VALIDTOR_DOMAIN_HXX
#define DBL_VALIDTOR_DOMAIN_HXX

#include "dbl/types/types.hxx"

namespace dbl {
namespace validator {
namespace domain {

bool is_valid(const std::string& name);
bool is_valid(const std::string& name, types::Errors_t& errors);

} // domain
} // validator
} //dbl

#endif

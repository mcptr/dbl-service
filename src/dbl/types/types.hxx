#ifndef DBL_TYPES_TYPES_HXX
#define DBL_TYPES_TYPES_HXX

#include "domain_list.hxx"
#include "domain.hxx"
#include <string>
#include <vector>

namespace dbl {
namespace types {

typedef std::vector<std::string> Names_t;
typedef std::vector<std::string> Errors_t;

typedef std::vector<DomainList> DomainListsSet_t;
typedef std::vector<Domain> DomainSet_t;

} // types
} // dbl

#endif

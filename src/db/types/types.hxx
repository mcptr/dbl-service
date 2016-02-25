#ifndef DBL_DB_TYPES_TYPES_HXX
#define DBL_DB_TYPES_TYPES_HXX

#define SOCI_USE_BOOST

#include "domain_lists.hxx"
#include "domains.hxx"

namespace dbl {
namespace db {
namespace types {

typedef std::vector<DomainList> DomainListsSet_t;
typedef std::vector<Domain> DomainSet_t;

} // types
} // db
} // dbl

#endif

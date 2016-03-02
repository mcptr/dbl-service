#ifndef DBL_DB_DML_QUERIES_HXX
#define DBL_DB_DML_QUERIES_HXX

#include <string>

namespace dbl {
namespace dml {
namespace queries {


const std::string get_blocked_domains = (
	"SELECT d.id, d.list_id, d.name,"
	"  d.description, dl.name AS list_name"
	"  FROM domains d"
	"  LEFT JOIN domain_lists dl ON dl.id = d.list_id"
	"  AND dl.active = 1"
	"  WHERE d.name NOT IN ("
	"     SELECT wd.name from whitelisted_domains wd)"
);

const std::string get_domains = (
	"SELECT d.id, d.list_id, d.name,"
	"  d.description, dl.name as list_name"
	"  FROM domains d"
	"  JOIN domain_lists dl ON "
	"    (dl.active = 1 AND dl.id = d.list_id)"
);

const std::string get_domain_lists = (
	"SELECT id, name, active, custom, description "
	"  FROM domain_lists"
);

const std::string get_whitelisted_domains = (
	"SELECT id, name, active, list_id, description,"
	"  '' as list_name"
	"  FROM whitelisted_domains"
	"  WHERE active = 1"
	"  ORDER BY name asc"
);


} // queries
} // dml
} // db

#endif

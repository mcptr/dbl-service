#ifndef DBL_DB_DML_QUERIES_HXX
#define DBL_DB_DML_QUERIES_HXX

#include <string>

namespace dbl {
namespace dml {
namespace queries {


const std::string get_blocked_domains = (
	"SELECT d.list_id, d.name,"
	"  dl.name AS list_name"
	"  FROM domains d"
	"  LEFT JOIN domain_lists dl ON dl.id = d.list_id"
	"  AND dl.active = 1"
	"  WHERE d.name NOT IN ("
	"     SELECT wd.name from whitelisted_domains wd)"
	" ORDER by d.name asc"
);

const std::string get_domains = (
	"SELECT d.list_id, d.name,"
	"  dl.name as list_name"
	"  FROM domains d"
	"  JOIN domain_lists dl ON "
	"    (dl.active = 1 AND dl.id = d.list_id)"
);

const std::string get_domain_lists = (
	"SELECT id, name, url, active, custom, mtime, description "
	"  FROM domain_lists"
);

const std::string get_whitelisted_domains = (
	"SELECT null as list_id, null as list_name, name"
	"  FROM whitelisted_domains"
	"  ORDER BY name asc"
);


} // queries
} // dml
} // db

#endif


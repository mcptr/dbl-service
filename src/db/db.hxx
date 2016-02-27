#ifndef DBL_DB_DB_HXX
#define DBL_DB_DB_HXX

#define SOCI_USE_BOOST

#include "types/types.hxx"

#include <string>
#include <vector>
#include <soci/soci.h>
#include <memory>

namespace dbl {
namespace db {

class DB
{
public:
	typedef std::vector<std::string> NamesList_t;

	DB() = delete;
	explicit DB(const std::string& dbpath, std::size_t pool_size = 2);
	~DB();

	void init();

	std::unique_ptr<db::types::DomainListsSet_t>
	get_domain_lists();

	std::unique_ptr<db::types::DomainList>
	get_domain_list_by_name(const std::string& name);

	std::unique_ptr<db::types::DomainListsSet_t>
	get_active_domain_lists();

	std::unique_ptr<db::types::DomainSet_t>
	get_domains(bool active_only = false);

	std::unique_ptr<db::types::Domain>
	get_domain(const std::string& name);

	std::unique_ptr<db::types::DomainSet_t>
	get_whitelisted_domains();

	std::unique_ptr<db::types::DomainSet_t>
	get_blocked_domains();

	// std::unique_ptr<db::types::DomainSet_t>
	// get_blocked_domains(const NamesList_t& lists);

	void import_list(const types::DomainList& lst, bool custom = false);
	void block_domains(const NamesList_t& domains, int list_id = 0);
	void unblock_domains(const NamesList_t& domains);

	int create_domain_list(const std::string& name,
						   const std::string& description = std::string(),
						   bool custom = false);

private:
	const std::string db_path_;
	const std::size_t pool_size_;
	soci::connection_pool pool_;

	const std::string get_blocked_domains_query = (
		"SELECT d.id, d.list_id, d.name,"
		"  d.description, dl.name AS list_name"
		"  FROM domains d"
		"  LEFT JOIN domain_lists dl ON dl.id = d.list_id"
		"  AND dl.active = 1"
		"  WHERE d.name NOT IN ("
		"     SELECT wd.name from whitelisted_domains wd)"
	);

	const std::string get_domains_query = (
		"SELECT d.id, d.list_id, d.name,"
		"  d.description, dl.name as list_name"
		"  FROM domains d"
		"  JOIN domain_lists dl ON "
		"    (dl.active = 1 AND dl.id = d.list_id)"
	);

	const std::string get_domain_lists_query = (
		"SELECT id, name, active, custom, description "
		"  FROM domain_lists"
	);

	const std::string get_whitelisted_domains_query = (
		"SELECT id, name, active, list_id, description,"
		"  '' as list_name"
		"  FROM whitelisted_domains"
		"  WHERE active = 1"
		"  ORDER BY name asc"
	);
};

} // db
} // dbl

#endif

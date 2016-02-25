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

	std::unique_ptr<db::types::DomainSet_t>
	get_blocked_domains(const NamesList_t& lists_names);
private:
	const std::string db_path_;
	const std::size_t pool_size_;
	soci::connection_pool pool_;

	const std::string get_blocked_domains_query = (
		"SELECT d.id, d.list_id, d.name, d.active, "
		"  d.description, dl.name as list_name"
		"  FROM domains d"
		"  JOIN domain_lists dl ON dl.id = d.list_id"
		"  WHERE d.name NOT IN ("
		"     SELECT wd.name from whitelisted_domains wd)"
	);

	const std::string get_domains_query = (
		"SELECT d.id, d.list_id, d.name, d.active, "
		"  d.description, dl.name as list_name"
		"  FROM domains d"
		"  JOIN domain_lists dl on dl.id = d.list_id"
	);
};

} // db
} // dbl

#endif

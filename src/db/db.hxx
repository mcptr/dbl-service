#ifndef DBL_DB_DB_HXX
#define DBL_DB_DB_HXX

#define SOCI_USE_BOOST

#include "types.hxx"

#include <string>
#include <vector>
#include <soci/soci.h>
#include <memory>

namespace dbl {

class DB
{
public:
	DB() = delete;
	explicit DB(const std::string& dbpath, int pool_size = 5);
	~DB() = default;

	void init();

	std::unique_ptr<dbtypes::DomainListsSet_t>
	get_domain_lists();

	std::unique_ptr<dbtypes::DomainListsSet_t>
	get_active_domain_lists();

	std::unique_ptr<dbtypes::DomainSet_t>
	get_domains(bool active_only = false);
private:
	soci::connection_pool pool_;
};

} // dbl

#endif

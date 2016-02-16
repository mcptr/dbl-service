#include "db.hxx"
#include "ddl.hxx"
#include "initial_data.hxx"
#include "types.hxx"
#include "core/common.hxx"


namespace dbl {

DB::DB(const std::string& dbpath, int pool_size)
	: pool_(soci::connection_pool(pool_size))
{
	for(int i = 0; i < pool_size; i++) {
		soci::session& sql = pool_.at(i);
		sql.open("sqlite3://" + dbpath);
		sql << "PRAGMA foreign_keys = ON;";
	}
}

void DB::init()
{
	soci::session sql(pool_);
	sql << DDL::settings_table;
	sql << DDL::domain_lists_table;
	sql << DDL::domains_table;

	int total_lists = 0;
	sql << "SELECT count(*) AS cnt FROM domain_lists", soci::into(total_lists);
	if(!total_lists) {
		for(auto const& q : dbl::data::domain_lists) {
			LOG(INFO) << q;
			sql << q;
		}

		for(auto const& q : dbl::data::domains) {
			LOG(INFO) << q;
			sql << q;
		}
	}
}

std::unique_ptr<dbtypes::DomainListsSet_t>
DB::get_domain_lists()
{
	using namespace soci;
	using dbtypes::DomainListsSet_t;
	using dbtypes::DomainList;

	std::unique_ptr<DomainListsSet_t> ptr(new DomainListsSet_t());
	session sql(pool_);

	DomainList record;
	DomainListsSet_t records;

	statement st = (
		sql.prepare << (
			"SELECT id, name, active, custom, description "
			"FROM domain_lists"
		),
		into(record)
	);

	st.execute();
	while(st.fetch()) {
		ptr->push_back(record);
	}

	return std::move(ptr);
}

std::unique_ptr<dbtypes::DomainSet_t>
DB::get_domains(bool active_only)
{
	using namespace soci;
	using dbtypes::DomainSet_t;

	std::unique_ptr<DomainSet_t> ptr(new DomainSet_t());
	session sql(pool_);

	Domain record;
	DomainSet_t records;

	std::string q(
		"SELECT id, list_id, name, active, description"
		"  FROM domains"
	);

	if(active_only) {
		q.append(" WHERE active = 1");
	}
	statement st = (sql.prepare << q, into(record));

	st.execute();
	while(st.fetch()) {
		ptr->push_back(record);
	}

	return std::move(ptr);
}

} // dbl

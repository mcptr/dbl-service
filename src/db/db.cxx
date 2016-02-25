#include "db.hxx"
#include "ddl/ddl.hxx"
#include "data/initial_data.hxx"
#include "core/common.hxx"

#include <soci/error.h>

namespace dbl {
namespace db {

DB::DB(const std::string& dbpath, std::size_t pool_size)
	: db_path_(dbpath),
	  pool_size_(pool_size),
	  pool_(soci::connection_pool(pool_size))
{
}

DB::~DB()
{
	for(std::size_t i = 0; i < pool_size_; i++) {
		soci::session& sql = pool_.at(i);
		sql.close();
	}
}

void DB::init()
{
	LOG(INFO) << "Opening database: " << db_path_;
	for(std::size_t i = 0; i < pool_size_; i++) {
		soci::session& sql = pool_.at(i);
		sql.open("sqlite3://" + db_path_);
		sql << "PRAGMA foreign_keys = ON;";
	}

	soci::session sql(pool_);
	sql.begin();

	sql << DDL::settings_table;
	sql << DDL::domain_lists_table;
	sql << DDL::domains_table;
	sql << DDL::whitelisted_domains_table;

	int total_lists = 0;
	sql << "SELECT count(*) AS cnt FROM domain_lists",
		soci::into(total_lists);

	if(!total_lists) {
		for(auto const& q : dbl::db::data::domain_lists) {
			LOG(INFO) << q;
			sql << q;
		}

		for(auto const& q : dbl::db::data::domains) {
			LOG(INFO) << q;
			sql << q;
		}
	}

	sql.commit();
}

std::unique_ptr<db::types::DomainListsSet_t>
DB::get_domain_lists()
{
	using namespace soci;
	using db::types::DomainListsSet_t;
	using db::types::DomainList;

	std::unique_ptr<DomainListsSet_t> ptr(new DomainListsSet_t());
	session sql(pool_);

	DomainList record;

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

std::unique_ptr<db::types::DomainSet_t>
DB::get_domains(bool active_only)
{
	using namespace soci;
	using db::types::DomainSet_t;

	std::unique_ptr<DomainSet_t> ptr(new DomainSet_t());
	session sql(pool_);

	Domain record;

	std::string q(get_domains_query);

	if(active_only) {
		q.append(" WHERE active = 1");
	}

	q.append(" ORDER BY name asc");
	statement st = (sql.prepare << q, into(record));

	st.execute();
	while(st.fetch()) {
		ptr->push_back(record);
	}

	return std::move(ptr);
}

std::unique_ptr<db::types::Domain>
DB::get_domain(const std::string& name)
{
	using namespace soci;
	using db::types::Domain;
	std::unique_ptr<Domain> record(new Domain());

	std::string q(get_domains_query + "  WHERE d.name = ?");

	soci::session sql(pool_);
	sql << q, use(name), into(*record);
	return std::move(record);
}

std::unique_ptr<db::types::DomainSet_t>
DB::get_whitelisted_domains()
{
	using namespace soci;
	using db::types::DomainSet_t;

	std::unique_ptr<DomainSet_t> ptr(new DomainSet_t());
	session sql(pool_);

	Domain record;

	std::string q(
		"SELECT id, name, active, list_id, description, '' as list_name"
		"  FROM whitelisted_domains ORDER BY name asc"
	);

	statement st = (sql.prepare << q, into(record));

	st.execute();
	while(st.fetch()) {
		ptr->push_back(record);
	}

	return std::move(ptr);
}

std::unique_ptr<db::types::DomainSet_t>
DB::get_blocked_domains()
{
	using namespace soci;
	using db::types::DomainSet_t;

	std::unique_ptr<DomainSet_t> ptr(new DomainSet_t());

	Domain record;
	session sql(pool_);
	LOG(INFO) << get_blocked_domains_query;
	statement st = (
		sql.prepare << get_blocked_domains_query,
		into(record)
	);

	st.execute();
	while(st.fetch()) {
		ptr->push_back(record);
	}

	return std::move(ptr);
}

std::unique_ptr<db::types::DomainSet_t>
DB::get_blocked_domains(const DB::NamesList_t& lists_names)
{
	using namespace soci;
	using db::types::DomainSet_t;

	std::unique_ptr<DomainSet_t> ptr(new DomainSet_t());
	std::string q = (
		get_blocked_domains_query +
		"  WHERE dl.name IN (?) "
	);

	Domain record;
	session sql(pool_);
	statement st = (sql.prepare << q, use(lists_names), into(record));

	st.execute();
	while(st.fetch()) {
		ptr->push_back(record);
	}

	return std::move(ptr);
}

} // db
} // dbl

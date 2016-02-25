#include "db.hxx"
#include "ddl.hxx"
#include "data/initial_data.hxx"
#include "core/common.hxx"


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
	sql << db::DDL::settings_table;
	sql << db::DDL::domain_lists_table;
	sql << db::DDL::domains_table;

	int total_lists = 0;
	sql << "SELECT count(*) AS cnt FROM domain_lists", soci::into(total_lists);
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

std::unique_ptr<db::types::DomainSet_t>
DB::get_domains(bool active_only)
{
	using namespace soci;
	using db::types::DomainSet_t;

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

	q.append(" ORDER BY name asc");
	statement st = (sql.prepare << q, into(record));

	st.execute();
	while(st.fetch()) {
		ptr->push_back(record);
	}

	return std::move(ptr);
}

} // db
} // dbl

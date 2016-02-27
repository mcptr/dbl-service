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
	LOG(DEBUG) << "Opening database: " << db_path_;
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
			sql << q;
		}

		for(auto const& q : dbl::db::data::domains) {
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
		sql.prepare << get_domain_lists_query,
		into(record)
	);

	st.execute();
	while(st.fetch()) {
		ptr->push_back(record);
	}

	return std::move(ptr);
}

std::unique_ptr<db::types::DomainList>
DB::get_domain_list_by_name(const std::string& name)
{
	std::unique_ptr<db::types::DomainList> ptr(
		new db::types::DomainList()
	);
	std::string q(get_domain_lists_query + " WHERE name = ?");
	soci::session sql(pool_);
	sql << q, soci::use(name), soci::into(*ptr);
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

	q.append(" ORDER BY d.name asc");
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
	statement st = (
		sql.prepare << get_whitelisted_domains_query,
		into(record)
	);

	st.execute(true);
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
	using db::types::Domain;

	std::unique_ptr<DomainSet_t> ptr(new DomainSet_t());

	Domain record;

	session sql(pool_);
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

// std::unique_ptr<db::types::DomainSet_t>
// DB::get_blocked_domains(const DB::NamesList_t& lists)
// {
// 	using namespace soci;
// 	using db::types::DomainSet_t;

// 	std::unique_ptr<DomainSet_t> ptr(new DomainSet_t());
// 	std::string q = (
// 		get_blocked_domains_query +
// 		"  WHERE dl.name IN (?) "
// 	);

// 	Domain record;
// 	session sql(pool_);
// 	statement st = (sql.prepare << q, use(lists), into(record));

// 	st.execute(true);
// 	while(st.fetch()) {
// 		ptr->push_back(record);
// 	}

// 	return std::move(ptr);
// }

void DB::block_domains(const NamesList_t& domains, int list_id)
{
	if(domains.empty()) {
		return;
	}

	if(!list_id) {
		list_id = create_domain_list("CUSTOM", "Custom list", true);
	}

	soci::session sql(pool_);
	sql.begin();

	soci::statement st_del = (
		sql.prepare << "DELETE FROM whitelisted_domains WHERE name = ?",
		soci::use(domains)
	);
	st_del.execute(true);

	std::string q(
		"INSERT OR REPLACE INTO domains(name, list_id)"
		"  VALUES(?, " + std::to_string(list_id) + ")"
	);

	soci::statement st_ins = (sql.prepare << q, soci::use(domains));
	st_ins.execute(true);
	sql.commit();
}

void DB::import_list(const types::DomainList& lst, bool custom)
{
	using namespace soci;
	int list_id = create_domain_list(
		lst.name,
		(lst.description.is_initialized() ? lst.description.get() : ""),
		custom
	);

	session sql(pool_);
	sql.begin();

	std::string q(
		"INSERT OR REPLACE INTO domains(name, description, list_id)"
		"  VALUES(?, ?, ?)"
	);

	std::string name;
	std::string description;
	statement st_ins = (
		sql.prepare << q, use(name), use(description), use(list_id)
	);
	for(auto const& domain : lst.domains) {
		LOG(INFO) << "IMPORTING DOMAIN: " << domain.name;
		name = domain.name;
		description = (
			domain.description.is_initialized() 
			? domain.description.get() : ""
		);
		st_ins.execute(true);
	}
	sql.commit();
}

void DB::unblock_domains(const NamesList_t& domains)
{
	soci::session sql(pool_);
	sql.begin();
	std::string q(
		"INSERT OR IGNORE INTO whitelisted_domains(name)"
		"  VALUES(?)"
	);

	soci::statement st = (sql.prepare << q, soci::use(domains));
	st.execute(true);
	sql.commit();
}

int DB::create_domain_list(const std::string& name,
						   const std::string& description,
						   bool custom)
{
	using namespace soci;
	using db::types::DomainList;

	std::string q(
		"INSERT OR IGNORE INTO domain_lists(name, description, custom)"
		"  VALUES(?, ?, ?)"
	);

	std::string list_name(name.empty() ? "CUSTOM" : name);

	int is_custom = custom;
	session sql(pool_);
	sql.begin();
	sql << q, use(list_name), use(description), use(is_custom);
	sql.commit();

	auto ptr = get_domain_list_by_name(list_name);
	return ptr->id;
}

} // db
} // dbl

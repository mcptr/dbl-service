#include "db.hxx"
#include "ddl/ddl.hxx"
#include "data/initial_data.hxx"
#include "dbl/core/common.hxx"

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

	sql << DDL::settings_table_schema;
	sql << DDL::domain_lists_table_schema;
	sql << DDL::domains_table_schema;
	sql << DDL::whitelisted_domains_table_schema;
	sql << DDL::stats_domains_table_schema;

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

DB::DBSession_ptr_t DB::session()
{
	DBSession_ptr_t ptr(new soci::session(pool_));
	return std::move(ptr);
}

} // db
} // dbl

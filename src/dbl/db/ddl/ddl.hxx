#ifndef DBL_DB_DDL_HXX
#define DBL_DB_DDL_HXX

#include <string>
#include <vector>

namespace dbl {
namespace db {
namespace DDL {

const std::string settings_table_schema =
	"CREATE TABLE IF NOT EXISTS settings("
	"  name VARCHAR(255) NOT NULL UNIQUE, "
	"  value VARCHAR(255) DEFAULT ''"
	")";


// WARNING: no unique constaint on url, although
// in sqlite3 it would work.
const std::string domain_lists_table_schema =
	"CREATE TABLE IF NOT EXISTS domain_lists("
	"  id INTEGER NOT NULL PRIMARY KEY, "
	"  name VARCHAR(255) NOT NULL UNIQUE, "
	"  url VARCHAR(255) DEFAULT NULL, "
	"  description VARCHAR(255) DEFAULT '', "
	"  active BOOLEAN NOT NULL DEFAULT 1, "
	"  custom BOOLEAN NOT NULL DEFAULT 0, "
	"  mtime INTEGER DEFAULT 0"
	")";

const std::string domains_table_schema =
	"CREATE TABLE IF NOT EXISTS domains("
	"  name VARCHAR(255) NOT NULL PRIMARY KEY, "
	"  list_id INTEGER NOT NULL, "
	"  FOREIGN KEY(list_id) REFERENCES domain_lists(id) "
	"    ON UPDATE CASCADE ON DELETE CASCADE"
	")";

const std::string whitelisted_domains_table_schema =
	"CREATE TABLE IF NOT EXISTS whitelisted_domains("
	"  name VARCHAR(255) NOT NULL UNIQUE "
	")";

const std::string stats_domains_table_schema =
	"CREATE TABLE IF NOT EXISTS stats_domains("
	"  name VARCHAR(255) NOT NULL UNIQUE, "
	"  hits INTEGER DEFAULT 0 "
	")";

} // DDL
} // db
} // dbl

#endif

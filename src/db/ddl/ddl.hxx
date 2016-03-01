#ifndef DBL_DB_DDL_HXX
#define DBL_DB_DDL_HXX

namespace dbl {
namespace db {
namespace DDL {

const std::string settings_table_schema =
	"CREATE TABLE IF NOT EXISTS settings("
	"  name VARCHAR(255) NOT NULL UNIQUE, "
	"  value VARCHAR(255) DEFAULT ''"
	")";

const std::string domain_lists_table_schema =
	"CREATE TABLE IF NOT EXISTS domain_lists("
	"  id INTEGER NOT NULL PRIMARY KEY, "
	"  name VARCHAR(255) NOT NULL UNIQUE, "
	"  description VARCHAR(255) DEFAULT '', "
	"  active BOOLEAN NOT NULL DEFAULT 1, "
	"  custom BOOLEAN NOT NULL DEFAULT 0"
	")";

const std::string domains_table_schema =
	"CREATE TABLE IF NOT EXISTS domains("
	"  id INTEGER NOT NULL PRIMARY KEY, "
	"  list_id INTEGER NOT NULL, "
	"  name VARCHAR(255) NOT NULL UNIQUE, "
	"  description VARCHAR(255) DEFAULT '', "
	"  FOREIGN KEY(list_id) REFERENCES domain_lists(id) "
	"    ON UPDATE CASCADE ON DELETE CASCADE"
	")";

const std::string whitelisted_domains_table_schema =
	"CREATE TABLE IF NOT EXISTS whitelisted_domains("
	"  id INTEGER NOT NULL PRIMARY KEY, "
	"  name VARCHAR(255) NOT NULL UNIQUE, "
	"  description VARCHAR(255) DEFAULT '' "
	")";

} // DDL
} // db
} // dbl

#endif

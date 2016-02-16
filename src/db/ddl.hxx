#ifndef DBL_DB_DDL_HXX
#define DBL_DB_DDL_HXX

namespace dbl {
namespace DDL {

const std::string settings_table = 
	"CREATE TABLE IF NOT EXISTS settings("
	"  name VARCHAR(255) NOT NULL UNIQUE, "
	"  value VARCHAR(255) DEFAULT NULL"
	")";

const std::string domain_lists_table = 
	"CREATE TABLE IF NOT EXISTS domain_lists("
	"  id INTEGER NOT NULL PRIMARY KEY, "
	"  name VARCHAR(255) NOT NULL UNIQUE, "
	"  description VARCHAR(255) DEFAULT NULL, "
	"  active BOOLEAN NOT NULL DEFAULT TRUE, "
	"  custom BOOLEAN NOT NULL DEFAULT FALSE"
	")";

const std::string domains_table = 
	"CREATE TABLE IF NOT EXISTS domains("
	"  id INTEGER NOT NULL PRIMARY KEY, "
	"  list_id INTEGER NOT NULL, "
	"  name VARCHAR(255) NOT NULL UNIQUE, "
	"  description VARCHAR(255) DEFAULT NULL, "
	"  active BOOLEAN DEFAULT TRUE, "
	"  FOREIGN KEY(list_id) REFERENCES domain_lists(id) "
	"    ON UPDATE CASCADE ON DELETE CASCADE"
	")";

} // DDL
} // dbl

#endif

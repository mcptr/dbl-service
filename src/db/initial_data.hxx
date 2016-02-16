#ifndef DBL_DB_INITIAL_DATA_HXX
#define DBL_DB_INITIAL_DATA_HXX

#include <string>
#include <vector>

namespace dbl {
namespace data {

std::vector<std::string> domain_lists = {
	"INSERT INTO domain_lists(name, description, custom)"
	"  VALUES('Default', 'Default, uncategorized list', 1)",
};

std::vector<std::string> domains = {
	"INSERT OR REPLACE INTO domains(name, list_id, description)"
	"  VALUES('bogus.example.com', 1, 'Bogus domain')",
};

} // data
} // dbl

#endif

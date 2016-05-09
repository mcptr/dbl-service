#ifndef DBL_DB_INITIAL_DATA_HXX
#define DBL_DB_INITIAL_DATA_HXX

#include <string>
#include <vector>

namespace dbl {
namespace db {
namespace data {

std::vector<std::string> domain_lists = {
	"INSERT INTO domain_lists(name, description, custom)"
	"  VALUES('CUSTOM', 'Custom, uncategorized list', 1)",

	"INSERT INTO domain_lists(name, description, custom, is_whitelist)"
	"  VALUES('WHITELIST', 'Custom, default whitelist', 1, 1)",
};

std::vector<std::string> domains = {
};

} // data
} // db
} // dbl

#endif

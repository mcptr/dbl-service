#ifndef DBL_DB_TYPES_DOMAIN_LISTS_HXX
#define DBL_DB_TYPES_DOMAIN_LISTS_HXX

#include "db/utils/utils.hxx"
#include "domains.hxx"

#include <soci/soci.h>
#include <boost/optional.hpp>
#include <string>
#include <ctime>


namespace dbl {
namespace db {
namespace types {

class DomainList
{
public:
	int id = int();
	std::string name = std::string();
	boost::optional<std::string> description = std::string();
	int active = int();
	int custom = int();

	std::vector<Domain> domains;

	bool from_json(const std::string& input);
};

} // types
} // db
} // dbl

namespace soci {

using dbl::db::types::DomainList;

template<>
struct type_conversion<DomainList>
{
	typedef values base_type;
	
	static
	void from_base(values const& v, indicator /* ind */, DomainList& r)
	{
		r.id = v.get<int>("id");
		r.name = v.get<std::string>("name");
		r.active = v.get<int>("active");
		r.custom = v.get<int>("custom");
		r.description = v.get<boost::optional<std::string>>("description");
	}
	
	static
	void to_base(const DomainList& r, soci::values& v, soci::indicator& ind)
	{
		using namespace dbl::db::utils;

		v.set("id", r.id);
		v.set("name", r.name);
		v.set("active", r.active);
		v.set("custom", r.custom);
		set_optional_value(v, "description", r.description);

		ind = i_ok;
	}
};

} // soci


#endif

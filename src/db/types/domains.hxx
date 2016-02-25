#ifndef DBL_DB_TYPES_DOMAINS_HXX
#define DBL_DB_TYPES_DOMAINS_HXX

#include "db/utils/utils.hxx"
#include <soci/soci.h>
#include <boost/optional.hpp>
#include <string>
#include <ctime>


namespace dbl {
namespace db {
namespace types {

class Domain
{
public:
	int id = int();
	int list_id = int();
	std::string name = std::string();
	boost::optional<std::string> description = std::string();
	int active = int();
};

} // types
} // db
} // dbl

namespace soci {

using dbl::db::types::Domain;

template<>
struct type_conversion<Domain>
{
	typedef values base_type;
	
	static
	void from_base(values const& v, indicator /* ind */, Domain& r)
	{
		r.id = v.get<int>("id");
		r.list_id = v.get<int>("list_id");
		r.name = v.get<std::string>("name");
		r.active = v.get<int>("active");
		r.description = v.get<boost::optional<std::string>>("description");
	}
	
	static
	void to_base(const Domain& r, soci::values& v, soci::indicator& ind)
	{
		using namespace dbl::db::utils;

		v.set("id", r.id);
		v.set("list_id", r.id);
		v.set("name", r.name);
		v.set("active", r.active);
		set_optional_value(v, "description", r.description);

		ind = i_ok;
	}
};

} // soci


#endif

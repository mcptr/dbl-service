#ifndef DBL_TYPES_DOMAIN_HXX
#define DBL_TYPES_DOMAIN_HXX

#include "db/utils/utils.hxx"
#include <soci/soci.h>
#include <boost/optional.hpp>
#include <string>
#include <ctime>

namespace dbl {
namespace types {

class Domain
{
public:
	int id = int();
	int list_id = int();
	std::string name = std::string();
	boost::optional<std::string> description = std::string();
	boost::optional<std::string> list_name = std::string();
};

} // types
} // dbl

namespace soci {

using dbl::types::Domain;

template<>
struct type_conversion<Domain>
{
	typedef values base_type;
	
	static
	void from_base(values const& v, indicator /* ind */, Domain& r)
	{
		r.id = v.get<int>("id");
		r.list_id = v.get<int>("list_id", 0);
		r.name = v.get<std::string>("name");
		r.description = v.get<boost::optional<std::string>>("description");
		r.list_name = v.get<boost::optional<std::string>>("list_name");
	}
	
	static
	void to_base(const Domain& r, soci::values& v, soci::indicator& ind)
	{
		using namespace dbl::db::utils;

		v.set("id", r.id);
		v.set("list_id", r.id);
		v.set("name", r.name);
		set_optional_value(v, "description", r.description);

		ind = i_ok;
	}
};

} // soci


#endif

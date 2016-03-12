#ifndef DBL_TYPES_DOMAIN_LIST_HXX
#define DBL_TYPES_DOMAIN_LIST_HXX

#include "domain.hxx"
#include "dbl/db/utils/utils.hxx"
#include "dbl/types/json_serializable.hxx"

#include <soci/soci.h>
#include <boost/optional.hpp>
#include <string>
#include <ctime>


namespace dbl {
namespace types {

class DomainList : public JSONSerializable
{
public:
	int id = int();
	std::string name = std::string();
	boost::optional<std::string> url = std::string();
	boost::optional<std::string> description = std::string();
	int active = int();
	int custom = int();
	int mtime = int();
	std::vector<Domain> domains;

	void init_from_json(const Json::Value& input);
	void to_json(Json::Value& root) const;
};

} // types
} // dbl

namespace soci {

using dbl::types::DomainList;

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
		r.mtime = v.get<int>("mtime");
		r.description = v.get<boost::optional<std::string>>("description");
		r.url = v.get<boost::optional<std::string>>("url");
	}
	
	static
	void to_base(const DomainList& r, soci::values& v, soci::indicator& ind)
	{
		using namespace dbl::db::utils;

		v.set("id", r.id);
		v.set("name", r.name);
		v.set("active", r.active);
		v.set("custom", r.custom);
		v.set("mtime", r.mtime);
		set_optional_value(v, "url", r.url);
		set_optional_value(v, "description", r.description);

		ind = i_ok;
	}
};

} // soci


#endif

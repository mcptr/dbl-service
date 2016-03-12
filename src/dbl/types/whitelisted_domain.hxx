#ifndef DBL_TYPES_WHITELISTED_DOMAIN_HXX
#define DBL_TYPES_WHITELISTED_DOMAIN_HXX

#include "json_serializable.hxx"
#include "dbl/db/utils/utils.hxx"
#include <soci/soci.h>
#include <boost/optional.hpp>
#include <string>
#include <ctime>

namespace dbl {
namespace types {

class WhitelistedDomain : public JSONSerializable
{
public:
	std::string name = std::string();

	void init_from_json(const Json::Value& input);
	void to_json(Json::Value& root) const;
};

} // types
} // dbl

namespace soci {

using dbl::types::WhitelistedDomain;

template<>
struct type_conversion<Domain>
{
	typedef values base_type;
	
	static
	void from_base(values const& v, indicator /* ind */, Domain& r)
	{
		r.name = v.get<std::string>("name");
	}
	
	static
	void to_base(const Domain& r, soci::values& v, soci::indicator& ind)
	{
		using namespace dbl::db::utils;

		v.set("name", r.name);
		ind = i_ok;
	}
};

} // soci


#endif

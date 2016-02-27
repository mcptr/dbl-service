#ifndef DBL_TYPES_TYPES_HXX
#define DBL_TYPES_TYPES_HXX

#include <string>
#include <json/json.h>

namespace dbl {
namespace types {

class JSONSerializable
{
public:
	virtual std::string to_json() const;
	virtual bool parse_json(const std::string& input);
	virtual bool from_json(const std::string& input) = 0;
protected:
	Json::Value json_root_;
};

} // types
} // dbl

#endif

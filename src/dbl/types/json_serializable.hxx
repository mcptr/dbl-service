#ifndef DBL_TYPES_JSON_SERIALIZABLE_HXX
#define DBL_TYPES_JSON_SERIALIZABLE_HXX

#include <string>
#include <json/json.h>

namespace dbl {
namespace types {

class JSONSerializable
{
public:
	virtual operator Json::Value() const final;

	virtual void from_json(const std::string& input) final;
	virtual void from_json(const Json::Value& input) final;
	virtual std::string to_json_string() const final;
protected:
	virtual void init_from_json(const Json::Value& root) = 0;
	virtual void to_json(Json::Value& root) const = 0;

private:
	virtual bool parse_json(const std::string& input,
							Json::Value& root) const final;

};

} // types
} // dbl

#endif

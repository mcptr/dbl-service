#include "json_serializable.hxx"
#include "core/common.hxx"

namespace dbl {
namespace types {

JSONSerializable::operator Json::Value() const
{
	return json_root_;
}

bool JSONSerializable::parse_json(const std::string& input)
{
	Json::Reader reader;

	bool success = reader.parse(input, json_root_);
	if(!success) {
		LOG(ERROR) << reader.getFormattedErrorMessages();
		return false;
	}
	return true;
}

std::string JSONSerializable::to_json() const
{
	return json_root_.toStyledString();
}


} // types
} // dbl

#include "json_serializable.hxx"
#include "dbl/core/common.hxx"

namespace dbl {
namespace types {

JSONSerializable::operator Json::Value() const
{
	Json::Value root;
	this->to_json(root);
	return root;
}

bool JSONSerializable::parse_json(const std::string& input,
								  Json::Value& root) const
{
	Json::Reader reader;
	bool success = reader.parse(input, root);
	if(!success) {
		//LOG(ERROR) << reader.getFormattedErrorMessages();
		return false;
	}

	return true;
}

void JSONSerializable::from_json(const std::string& input)
{
	Json::Value root;
	if(this->parse_json(input, root)) {
		this->init_from_json(root);
	}
}

void JSONSerializable::from_json(const Json::Value& input)
{
	this->init_from_json(input);
}

std::string JSONSerializable::to_json_string() const
{
	Json::Value root;
	this->to_json(root);
	return root.toStyledString();
}


} // types
} // dbl

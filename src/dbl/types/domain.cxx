#include "domain.hxx"
#include "dbl/core/common.hxx"
#include "dbl/types/types.hxx"
#include "dbl/validator/domain.hxx"

namespace dbl {
namespace types {

bool Domain::from_json(const std::string& input)
{
	using namespace validator::domain;

	if(!this->parse_json(input)) {
		return false;
	}

	else if(!json_root_.isObject()) {
		LOG(ERROR) << "Invalid format, expected object at root";
		return false;
	}

	if(!json_root_["name"].empty()) {
		types::Errors_t errors;
		if(is_valid(json_root_["name"].asString(), errors)) {
			name = json_root_["name"].asString();
			description = json_root_["description"].asString();
		}
		else {
			for(auto const& err : errors) {
				LOG(WARNING) << json_root_["name"].asString()
							 << ": " << err;
			}
		}
	}

	return true;
}

Domain::operator Json::Value() const
{
	Json::Value root;
	root["name"] = name;
	if(description.is_initialized()) {
		root["description"] = description.get();
	}
	return root;
}

} // types
} // dbl

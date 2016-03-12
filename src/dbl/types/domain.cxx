#include "domain.hxx"
#include "dbl/core/common.hxx"
#include "dbl/types/types.hxx"
#include "dbl/validator/domain.hxx"

namespace dbl {
namespace types {

void Domain::init_from_json(const Json::Value& root)
{
	using namespace validator::domain;

	if(!root.isObject()) {
		LOG(ERROR) << "Invalid format, expected object at root";
		throw std::runtime_error("Invalid format");
	}

	if(!root["name"].empty()) {
		types::Errors_t errors;
		if(is_valid(root["name"].asString(), errors)) {
			name = root["name"].asString();
		}
		else {
			for(auto const& err : errors) {
				LOG(WARNING) << root["name"].asString()
							 << ": " << err;
			}
		}
	}
}

void Domain::to_json(Json::Value& root) const
{
	root["name"] = name;
	root["list_id"] = list_id;
	if(list_name.is_initialized()) {
		root["list_name"] = list_name.get();
	}
}

} // types
} // dbl

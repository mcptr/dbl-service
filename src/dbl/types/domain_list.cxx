#include "domain_list.hxx"
#include "dbl/core/common.hxx"
#include "dbl/types/types.hxx"
#include "dbl/validator/domain.hxx"

#include <json/json.h>

namespace dbl {
namespace types {

bool DomainList::from_json(const std::string& input)
{
	using namespace validator::domain;

	if(!this->parse_json(input)) {
		return false;
	}

	else if(!json_root_["domains"].isArray()) {
		LOG(ERROR) << "Invalid format, expected array at 'domains'";
		return false;
	}

	if(!json_root_["name"].empty()) {
		name = json_root_["name"].asString();
	}

	if(!json_root_["description"].empty()) {
		description = json_root_["description"].asString();
	}

	const Json::Value& entries = json_root_["domains"];
	for(auto const& item : entries) {
		if(!item["name"].empty()) {
			types::Errors_t errors;
			if(is_valid(item["name"].asString(), errors)) {
				Domain domain;
				domain.name = item["name"].asString();
				domain.description = item["description"].asString();
				domains.push_back(domain);
			}
			else {
				for(auto const& err : errors) {
					LOG(WARNING) << item["name"].asString()
								 << ": " << err;
				}
			}
		}
	}

	return true;
}

std::string DomainList::to_json() const
{
	Json::Value root;

	root["name"] = name;

	if(description.is_initialized()) {
		root["description"] = description.get();
	}

	root["domains"] = Json::arrayValue;

	for(auto const& domain : domains) {
		root["domains"].append(domain);
	}

	return root.toStyledString();
}


} // types
} // dbl

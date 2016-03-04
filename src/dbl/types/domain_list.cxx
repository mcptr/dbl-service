#include "domain_list.hxx"
#include "dbl/core/common.hxx"
#include "dbl/types/types.hxx"
#include "dbl/validator/domain.hxx"

#include <json/json.h>

namespace dbl {
namespace types {


void DomainList::init_from_json(const Json::Value& root)
{
	using namespace validator::domain;

	if(!root["name"].empty()) {
		name = root["name"].asString();
	}

	if(!root["description"].empty()) {
		description = root["description"].asString();
	}

	if(!root["domains"].empty() && !root["domains"].isArray()) {
		LOG(ERROR) << "Invalid format, expected array at 'domains'";
		throw std::runtime_error("Invalid format");
	}
	else {
		const Json::Value& entries = root["domains"];
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
	}
}

void DomainList::to_json(Json::Value& root) const
{
	root["name"] = name;

	if(description.is_initialized()) {
		root["description"] = description.get();
	}

	root["domains"] = Json::arrayValue;

	for(auto const& domain : domains) {
		root["domains"].append(domain);
	}
}


} // types
} // dbl

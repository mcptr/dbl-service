#include "domain_list.hxx"
#include "core/common.hxx"
#include "validator/types.hxx"
#include "validator/domain.hxx"

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
			validator::Errors_t errors;
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

} // types
} // dbl

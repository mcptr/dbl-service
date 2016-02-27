#include "domain_lists.hxx"
#include "core/common.hxx"
#include "validator/types.hxx"
#include "validator/domain.hxx"

#include <json/json.h>

namespace dbl {
namespace db {
namespace types {

bool DomainList::from_json(const std::string& input)
{
	using namespace validator::domain;

	Json::Reader reader;
	Json::Value root;
	bool success = reader.parse(input, root);
	if(!success) {
		LOG(ERROR) << reader.getFormattedErrorMessages();
		return false;
	}
	else if(!root["domains"].isArray()) {
		LOG(ERROR) << "Invalid format, expected array at 'domains'";
		return false;
	}

	if(!root["name"].empty()) {
		name = root["name"].asString();
	}

	if(!root["description"].empty()) {
		description = root["description"].asString();
	}

	const Json::Value& entries = root["domains"];
	for(auto const& item : entries) {
		if(!item["name"].empty()) {
			validator::Errors_t errors;
			if(is_valid(item["name"].asString(), errors)) {
				db::types::Domain domain;
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
} // db
} // dbl

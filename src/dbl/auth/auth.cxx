#include "auth.hxx"
#include "dbl/manager/settings_manager.hxx"
#include "dbl/util/crypto.hxx"

#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace dbl {
namespace auth {

Auth::Auth(std::shared_ptr<core::Api> api)
	: api_(api)
{
	static boost::uuids::random_generator rg;
	std::stringstream s;
	s << rg();
	token_ = s.str();
}

bool Auth::auth(const std::string& token,
				const std::string& response_hash)
{
	if(token_.compare(token) != 0) {
		return false;
	}

	manager::SettingsManager settings_mgr(api_);
	std::string passwd = settings_mgr.get("service_password", "");
	if(passwd.empty()) {
		LOG(WARNING) << "AUTH: No password set";
		return true;
	}
	else if(response_hash.empty()) {
		return false;
	}

	std::string expected(crypto::md5(passwd + token));
	return (expected.compare(response_hash) == 0);
}

bool Auth::set_password(const std::string& passwd_hash,
						const std::string& hashed_token,
						types::Errors_t& errors)
{
	if(passwd_hash.empty()) {
		errors.push_back("Cannot set empty password");
		return false;
	}

	if(passwd_hash.length() != 32) {
		errors.push_back("Password must be a md5 hash");
		return false;
	}

	std::string expected(crypto::md5(passwd_hash + token_));
	if(expected.compare(hashed_token) != 0) {
		errors.push_back("Token hashed with different password");
		return false;
	}

	manager::SettingsManager settings_mgr(api_);
	return settings_mgr.set("service_password", passwd_hash);
}

bool Auth::remove_password()
{
	manager::SettingsManager settings_mgr(api_);
	return settings_mgr.remove("service_password");
}


} // auth
} // dbl

#include "settings_manager.hxx"
#include "db/db.hxx"

namespace dbl {
namespace manager {

SettingsManager::SettingsManager(std::shared_ptr<core::Api> api)
	: Manager(api)
{
}

std::string SettingsManager::get(const std::string& key,
								 const std::string& def)
{
	using namespace soci;
	std::string value;
	try {
		auto session_ptr = api_->db()->session();

		*session_ptr << "SELECT value FROM settings WHERE name = ?",
			use(key), into(value); 
	}
	catch(const std::runtime_error& e) {
		LOG(ERROR) << e.what();
	}
	return (value.empty() ? def : value);
}

bool SettingsManager::set(const std::string& key, const std::string& value)
{
	using namespace soci;
	try {
		auto session_ptr = api_->db()->session();

		session_ptr->begin();
		*session_ptr << "INSERT OR REPLACE INTO settings(name, value) "
			<< "  VALUES(?, ?)",
			use(key), use(value); 
		session_ptr->commit();
		return true;
	}
	catch(const std::runtime_error& e) {
		LOG(ERROR) << e.what();
	}

	return false;
}

bool SettingsManager::remove(const std::string& key)
{
	using namespace soci;
	try {
		auto session_ptr = api_->db()->session();

		session_ptr->begin();
		*session_ptr << "DELETE FROM settings WHERE name = ?",use(key); 
		session_ptr->commit();
		return true;
	}
	catch(const std::runtime_error& e) {
		LOG(ERROR) << e.what();
	}

	return false;
}

} // manager
} // dbl

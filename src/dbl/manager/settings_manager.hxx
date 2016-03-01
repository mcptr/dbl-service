#ifndef DBL_MANAGER_SETTINGS_MANAGER_HXX
#define DBL_MANAGER_SETTINGS_MANAGER_HXX

#include "manager.hxx"
#include <string>

namespace dbl {
namespace manager {

class SettingsManager : public Manager
{
public:
	SettingsManager() = delete;
	SettingsManager(std::shared_ptr<core::Api> api);
	~SettingsManager() = default;

	std::string get(const std::string& key, const std::string& def);
	bool set(const std::string& key, const std::string& value);
	bool remove(const std::string& key);
};

} // manager
} // dbl


#endif

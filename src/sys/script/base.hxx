#ifndef DBL_SYS_SCRIPT_BASE_HXX
#define DBL_SYS_SCRIPT_BASE_HXX

#include "core/rtapi.hxx"

#include <string>
#include <unordered_map>

namespace dbl {

class BaseScript
{
public:
	typedef
	std::unordered_map<std::string, std::string> Environment_t;

	BaseScript() = delete;
	explicit BaseScript(std::shared_ptr<RTApi> api, const std::string& name);
	virtual ~BaseScript() = default;

	virtual void set_env(const std::string& var, const std::string& value);
	virtual bool run() = 0;

protected:
	std::shared_ptr<RTApi> api_;

	const std::string name_;
	std::string path_;

	Environment_t env_;
};

} // dbl

#endif

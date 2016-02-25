#ifndef DBL_SYS_SCRIPT_SCRIPT_HXX
#define DBL_SYS_SCRIPT_SCRIPT_HXX

#include "core/api.hxx"

#include <string>
#include <unordered_map>

namespace dbl {
namespace sys {
namespace script {

class Script
{
public:
	typedef
	std::unordered_map<std::string, std::string> Environment_t;

	Script() = delete;
	explicit Script(std::shared_ptr<core::Api> api, const std::string& name);
	virtual ~Script() = default;

	virtual void set_env(const std::string& var, const std::string& value);
	virtual bool run() final;

	virtual bool is_success() const final;
	virtual int get_status() const final;

protected:
	std::shared_ptr<core::Api> api_;
	const std::string name_;
	std::string path_;
	Environment_t env_;

	int status_ = 1;

	virtual int execute() const;
	virtual void export_env() const = 0;
	virtual void unexport_env() const = 0;
};

} // script
} // sys
} // dbl

#endif

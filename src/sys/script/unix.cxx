#include "unix.hxx"
#include "core/common.hxx"

#include <cstdlib>

namespace dbl {

Script::Script(std::shared_ptr<core::Api> api, const std::string& name)
	: BaseScript(api, name)
{
}

void Script::export_env() const
{
	for(auto const& it : env_) {
		std::string var("DBL_" + it.first);
		setenv(var.c_str(), it.second.c_str(), 1);
	}
}

void Script::unexport_env() const
{
	for(auto const& it : env_) {
		std::string var("DBL_" + it.first);
		unsetenv(var.c_str());
	}
}


} // dbl

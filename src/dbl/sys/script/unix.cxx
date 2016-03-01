#include "dbl/unix.hxx"
#include "dbl/core/common.hxx"

#include <cstdlib>

namespace dbl {
namespace sys {
namespace script {

Script::Script(std::shared_ptr<core::Api> api, const std::string& name)
	: Script(api, name)
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

} // script
} // sys
} // dbl

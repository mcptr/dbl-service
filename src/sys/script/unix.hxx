#ifndef DBL_SYS_SCRIPT_UNIX_HXX
#define DBL_SYS_SCRIPT_UNIX_HXX

#include "script.hxx"

namespace dbl {
namespace sys {
namespace script {

class Script : public Script
{
public:
	Script() = delete;
	explicit Script(std::shared_ptr<core::Api> api, const std::string& name);
	virtual ~Script() = default;

protected:
	virtual void export_env() const;
	virtual void unexport_env() const;
	
};

} // script
} // sys
} // dbl

#endif

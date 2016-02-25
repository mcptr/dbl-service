#ifndef DBL_SYS_SCRIPT_UNIX_HXX
#define DBL_SYS_SCRIPT_UNIX_HXX

#include "base.hxx"

namespace dbl {

class Script : public BaseScript
{
public:
	Script() = delete;
	explicit Script(std::shared_ptr<core::Api> api, const std::string& name);
	virtual ~Script() = default;

protected:
	virtual void export_env() const;
	virtual void unexport_env() const;
	
};

} // dbl

#endif

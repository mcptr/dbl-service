#ifndef DBL_SYS_SCRIPT_UNIX_HXX
#define DBL_SYS_SCRIPT_UNIX_HXX

#include "base.hxx"

namespace dbl {

class Script : public BaseScript
{
public:
	Script() = delete;
	explicit Script(std::shared_ptr<RTApi> api, const std::string& name);
	virtual ~Script() = default;

	virtual bool run();
};

} // dbl

#endif

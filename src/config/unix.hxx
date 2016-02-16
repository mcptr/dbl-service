#ifndef CONFIG_UNIX_HXX
#define CONFIG_UNIX_HXX

#include "base.hxx"
#include <string>

namespace dbl {

class UnixConfig : public BaseConfig
{
public:
	UnixConfig();
	UnixConfig(const UnixConfig& o) = default;
	virtual ~UnixConfig() = default;
};

} // dbl

#endif

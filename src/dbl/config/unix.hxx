#ifndef DBL_CONFIG_UNIX_HXX
#define DBL_CONFIG_UNIX_HXX

#include "config.hxx"
#include <string>

namespace dbl {
namespace config {

class Unix : public Config
{
public:
	Unix();
	Unix(const Unix& o) = default;
	virtual ~Unix() = default;
};

} // config
} // dbl

#endif

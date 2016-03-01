#ifndef DBL_MANAGER_MANAGER_HXX
#define DBL_MANAGER_MANAGER_HXX

#include "dbl/core/api.hxx"
#include <memory>

namespace dbl {
namespace manager {

class Manager
{
public:
	Manager() = delete;
	Manager(std::shared_ptr<core::Api> api);
	virtual ~Manager() = default;

protected:
	std::shared_ptr<core::Api> api_;
};

} // manager
} // dbl

#endif

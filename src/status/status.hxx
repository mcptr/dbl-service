#ifndef DBL_CORE_STATUS_HXX
#define DBL_CORE_STATUS_HXX

#include "core/api.hxx"
#include <memory>

namespace dbl {

class Status
{
public:
	Status() = delete;
	Status(std::shared_ptr<core::Api> api);
	~Status() = default;
private:
	std::shared_ptr<core::Api> api_;
};

} // dbl

#endif


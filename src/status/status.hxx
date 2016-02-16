#ifndef DBL_CORE_STATUS_HXX
#define DBL_CORE_STATUS_HXX

#include "core/rtapi.hxx"
#include <memory>

namespace dbl {

class Status
{
public:
	Status() = delete;
	Status(std::shared_ptr<RTApi> api);
	~Status() = default;

	void print_lists();
	void print_domains(bool active_only = false);
private:
	std::shared_ptr<RTApi> api_;
};

} // dbl

#endif


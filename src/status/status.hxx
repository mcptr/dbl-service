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

	void print_lists();
	void print_domains(bool active_only = false);
private:
	std::shared_ptr<core::Api> api_;
};

} // dbl

#endif


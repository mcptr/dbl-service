#include "manager.hxx"

namespace dbl {
namespace manager {

Manager::Manager(std::shared_ptr<core::Api> api)
	: api_(api)
{
}

} // manager
} // dbl

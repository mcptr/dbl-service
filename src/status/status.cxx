#include "status.hxx"
#include <iostream>

namespace dbl {

Status::Status(std::shared_ptr<core::Api> api)
	: api_(api)
{
}

} // dbl

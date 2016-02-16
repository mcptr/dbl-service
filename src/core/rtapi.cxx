#include "rtapi.hxx"

#include <string>

namespace dbl {

RTApi::RTApi(const BaseConfig& cfg,
			 std::shared_ptr<DB> db)
	: config(cfg),
	  db_(db)
{
}

std::shared_ptr<DB> RTApi::db()
{
	return db_;
}

} // dbl

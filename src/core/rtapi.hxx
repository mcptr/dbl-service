#ifndef DBL_CORE_RTAPI_HXX
#define DBL_CORE_RTAPI_HXX

#include "options/options.hxx"
#include "core/common.hxx"
#include "db/db.hxx"
#include "config/base.hxx"

#include <memory>

namespace dbl {

class RTApi
{
public:
	RTApi() = delete;
	RTApi(const BaseConfig& config,
		  std::shared_ptr<DB> db);
	~RTApi() = default;


	const BaseConfig& config;
	std::shared_ptr<DB> db();

private:
	std::shared_ptr<DB> db_;
};

} // service


#endif

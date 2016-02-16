#ifndef DBL_CORE_RTAPI_HXX
#define DBL_CORE_RTAPI_HXX

#include "options/options.hxx"
#include "core/common.hxx"
#include "db/db.hxx"

#include <string>
#include <memory>

namespace dbl {

class RTApi
{
public:
	RTApi() = delete;
	RTApi(const Options& po);
	~RTApi() = default;

	const Options program_options;
	std::shared_ptr<DB> db;
};

} // service


#endif

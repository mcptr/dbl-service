#ifndef DBL_CORE_API_HXX
#define DBL_CORE_API_HXX

#include "dbl/core/common.hxx"
#include "dbl/config/config.hxx"
#include "dbl/db/db.hxx"
#include "dbl/logger/logger.hxx"
#include "dbl/options/options.hxx"
#include "dbl/status/status.hxx"

#include <memory>

namespace dbl {
namespace core {

class Api
{
public:
	Api() = delete;
	Api(const config::Config& config,
		  std::shared_ptr<db::DB> db);
	~Api() = default;


	status::Status status;
	const config::Config& config;
	std::shared_ptr<db::DB> db();
	std::shared_ptr<logger::Logger> logger();
private:
	std::shared_ptr<db::DB> db_;
	std::shared_ptr<logger::Logger> logger_;
};

} // core
} // dbl


#endif

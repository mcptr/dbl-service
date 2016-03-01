#ifndef DBL_CORE_API_HXX
#define DBL_CORE_API_HXX

#include "dbl/options/options.hxx"
#include "dbl/core/common.hxx"
#include "dbl/config/config.hxx"
#include "dbl/db/db.hxx"

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


	const config::Config& config;
	std::shared_ptr<db::DB> db();

private:
	std::shared_ptr<db::DB> db_;
};

} // core
} // dbl


#endif

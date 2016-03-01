#include "api.hxx"

#include <string>

namespace dbl {
namespace core {

Api::Api(const config::Config& cfg,
			 std::shared_ptr<db::DB> db)
	: config(cfg),
	  db_(db)
{
}

std::shared_ptr<db::DB> Api::db()
{
	return db_;
}

} // core
} // dbl

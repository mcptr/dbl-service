#include "api.hxx"

#include <string>

namespace dbl {
namespace core {

Api::Api(const config::Config& cfg,
			 std::shared_ptr<db::DB> db)
	: config(cfg),
	  db_(db),
	  logger_(new logger::Logger(db))
{
}

std::shared_ptr<db::DB> Api::db()
{
	return db_;
}

std::shared_ptr<logger::Logger> Api::logger()
{
	return logger_;
}

} // core
} // dbl

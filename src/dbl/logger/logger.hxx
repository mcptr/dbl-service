#ifndef DBL_LOGGER_LOGGER_HXX
#define DBL_LOGGER_LOGGER_HXX

#include "dbl/db/db.hxx"
#include <string>
#include <unordered_map>

namespace dbl {
namespace logger {

class Logger
{
public:
	Logger() = delete;
	Logger(std::shared_ptr<db::DB> db);
	~Logger() = default;

	void log_domain_hit(const std::string& domain);

private:
	std::shared_ptr<db::DB> db_;
	std::unordered_map<std::string, bool> domains_seen_;
};

} // logger
} // dbl

#endif

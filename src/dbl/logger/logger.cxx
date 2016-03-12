#include "logger.hxx"
#include "dbl/core/common.hxx"
#include <vector>

namespace dbl {
namespace logger {

Logger::Logger(std::shared_ptr<db::DB> db)
	: db_(db)
{
}

void Logger::log_domain_hit(const std::string& domain)
{
	std::vector<std::string> queries;
	using namespace soci;

	try {
		auto session_ptr = db_->session();
		session_ptr->begin();

		if(!domains_seen_[domain]) {
			queries.push_back(
				"INSERT OR IGNORE INTO stats_domains(name) values(?)"
			);
		}

		queries.push_back(
			"UPDATE stats_domains SET hits = hits + 1 WHERE name = ?"
		);

		for(auto const& q : queries) {
			*session_ptr << q, soci::use(domain);
		}

		session_ptr->commit();
	}
	catch(const std::runtime_error& e) {
		LOG(ERROR) << e.what();
	}
}

} // logger
} // dbl

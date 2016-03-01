#ifndef DBL_DB_DB_HXX
#define DBL_DB_DB_HXX

#define SOCI_USE_BOOST

#include "types/types.hxx"

#include <string>
#include <vector>
#include <soci/soci.h>
#include <memory>

namespace dbl {
namespace db {

class DB
{
public:
	typedef	std::unique_ptr<soci::session> DBSession_ptr_t;


	DB() = delete;
	explicit DB(const std::string& dbpath, std::size_t pool_size = 2);
	~DB();

	void init();
	DBSession_ptr_t session();

private:
	const std::string db_path_;
	const std::size_t pool_size_;
	soci::connection_pool pool_;
};

} // db
} // dbl

#endif

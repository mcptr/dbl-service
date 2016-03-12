#ifndef DBL_MGMT_MGMT_HXX
#define DBL_MGMT_MGMT_HXX

#include "dbl/core/api.hxx"
#include "dbl/options/options.hxx"

#include <string>
#include <vector>
#include <memory>

namespace dbl {
namespace mgmt {

class Mgmt
{
public:
	Mgmt() = delete;
	Mgmt(std::shared_ptr<core::Api> api,
		 const Options po);
	~Mgmt() = default;

	bool has_work() const;
	bool manage();
private:
 	std::shared_ptr<core::Api> api_;
	const Options po_;

	bool add_list_;
	const std::string delete_list_;
	const std::string export_list_;
	const std::vector<std::string> block_domains_;
	const std::vector<std::string> unblock_domains_;

	bool add_list();
	bool delete_list();
	bool export_list();
	bool manage_domains();
};

} // mgmt
} // dbl

#endif

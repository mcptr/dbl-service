#ifndef DBL_QUERY_QUERY_HXX
#define DBL_QUERY_QUERY_HXX

#include "core/api.hxx"
#include "options/options.hxx"


namespace dbl {
namespace query {

class Query
{
public:
	Query() = delete;
	Query(const Options& po, std::shared_ptr<core::Api> api);
	~Query() = default;

	bool run();
private:
	const Options po_;
	std::shared_ptr<core::Api> api_;

	void print_domain_lists();
	void print_blocked_domains();
	bool print_domain_details(const std::string& domain);
};

} // query
} // dbl

#endif

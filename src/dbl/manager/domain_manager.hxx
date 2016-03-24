#ifndef DBL_MANAGER_DOMAIN_MANAGER_HXX
#define DBL_MANAGER_DOMAIN_MANAGER_HXX

#include "manager.hxx"
#include "dbl/types/types.hxx"

namespace dbl {
namespace manager {

class DomainManager : public Manager
{
public:
	DomainManager() = delete;
	DomainManager(std::shared_ptr<core::Api> api);
	~DomainManager() = default;

	std::unique_ptr<types::DomainSet_t> get(int list_id = 0);
	std::unique_ptr<types::Domain> get(const std::string& name);
	std::unique_ptr<types::DomainSet_t> get_blocked();
	std::unique_ptr<types::DomainSet_t> get_whitelisted();

	bool block_domains(const types::Names_t& domains, int list_id = 0);
	bool unblock_domains(const types::Names_t& domains);

	void filter_valid(const types::Names_t& domains,
					  types::Names_t& result);
};

} // manager
} // dbl

#endif

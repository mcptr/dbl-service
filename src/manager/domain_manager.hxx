#ifndef DBL_MANAGER_DOMAIN_MANAGER_HXX
#define DBL_MANAGER_DOMAIN_MANAGER_HXX

#include "manager.hxx"
#include "types/types.hxx"

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

	void block_domains(const types::Names_t& domains, int list_id = 0);
	void unblock_domains(const types::Names_t& domains);
};

} // manager
} // dbl

#endif

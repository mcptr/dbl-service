#ifndef DBL_MANAGER_DOMAIN_LIST_MANAGER_HXX
#define DBL_MANAGER_DOMAIN_LIST_MANAGER_HXX

#include "manager.hxx"
#include "dbl/types/types.hxx"

namespace dbl {
namespace manager {

class DomainListManager : public Manager
{
public:
	DomainListManager() = delete;
	DomainListManager(std::shared_ptr<core::Api> api);
	~DomainListManager() = default;

	bool import(const types::DomainList& lst, bool custom);
	bool import(const types::DomainListsSet_t& lst_set);

	int create(const std::string& name,
			   const std::string& description,
			   bool custom);

	// std::unique_ptr<types::DomainList>
	// export_list(const std::string& name);

	// std::unique_ptr<types::DomainListsSet_t>
	// export_list(const types::Names_t& names);

	std::unique_ptr<types::DomainListsSet_t> get();

	std::unique_ptr<types::DomainList>
	get(const std::string& name, bool with_domains = false);

};

} // manager
} // dbl

#endif

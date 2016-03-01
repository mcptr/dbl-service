#include "domain_list_manager.hxx"
#include "dbl/core/common.hxx"
#include "dbl/db/db.hxx"
#include "dbl/db/dml/queries.hxx"
#include "domain_manager.hxx"


namespace dbl {
namespace manager {

DomainListManager::DomainListManager(std::shared_ptr<core::Api> api)
	: Manager(api)
{
}

int DomainListManager::create(const std::string& name,
							  const std::string& description,
							  bool custom)
{
	using namespace soci;
	using types::DomainList;

	std::string q(
		"INSERT OR IGNORE INTO domain_lists(name, description, custom)"
		"  VALUES(?, ?, ?)"
	);

	std::string list_name(name.empty() ? "CUSTOM" : name);
	int is_custom = custom;
	try {
		auto session_ptr = api_->db()->session();
		session_ptr->begin();
		*session_ptr << q, use(list_name), use(description), use(is_custom);
		session_ptr->commit();
		session_ptr.reset();

		auto ptr = get(list_name);
		return ptr->id;
	}
	catch(const std::runtime_error& e) {
		LOG(ERROR) << e.what();
	}

	return 0;
}

bool DomainListManager::import(const types::DomainList& lst, bool custom)
{
	using namespace soci;

	try {
		int list_id = create(
			lst.name,
			(lst.description.is_initialized()
			 ? lst.description.get() : ""),
			custom
		);

		auto session_ptr = api_->db()->session();
		session_ptr->begin();

		std::string q(
			"INSERT OR REPLACE INTO domains(name, description, list_id)"
			"  VALUES(?, ?, ?)"
		);

		std::string name;
		std::string description;
		statement st_ins = (
			session_ptr->prepare << q, use(name), use(description), use(list_id)
		);
		for(auto const& domain : lst.domains) {
			LOG(INFO) << "IMPORTING DOMAIN: " << domain.name;
			name = domain.name;
			description = (
				domain.description.is_initialized() 
				? domain.description.get() : ""
			);
			st_ins.execute(true);
		}
		session_ptr->commit();
	}
	catch(const std::runtime_error& e) {
		LOG(ERROR) << e.what();
		return false;
	}

	return true;
}

bool DomainListManager::import(const types::DomainListsSet_t& lst_set)
{
	return false;
}

// std::unique_ptr<types::DomainList>
// DomainListManager::export_list(const std::string& name)
// {
// }

// DomainListManager::std::unique_ptr<types::DomainListsSet_t>
// export_list(const types::Names_t& names)
// {
// }

std::unique_ptr<types::DomainListsSet_t> DomainListManager::get()
{
	using namespace soci;
	using types::DomainListsSet_t;
	using types::DomainList;

	DomainList record;

	std::unique_ptr<DomainListsSet_t> ptr(new DomainListsSet_t());

	try {
		auto session_ptr = api_->db()->session();

		soci::statement st = (
			session_ptr->prepare << dml::queries::get_domain_lists,
			soci::into(record)
		);

		st.execute();
		while(st.fetch()) {
			ptr->push_back(record);
		}
	}
	catch(const std::runtime_error& e) {
		LOG(ERROR) << e.what();
	}

	return std::move(ptr);
}

std::unique_ptr<types::DomainList>
DomainListManager::get(const std::string& name, bool with_domains)
{
	using namespace soci;
	using types::DomainList;

	std::unique_ptr<DomainList> ptr(new DomainList());

	std::string q(
		dml::queries::get_domain_lists + " WHERE name = ?"
	);

	try {
		auto session_ptr = api_->db()->session();
		*session_ptr << q, use(name), into(*ptr);

		session_ptr.reset();

		if(ptr->id && with_domains) {
			DomainManager domain_mgr(api_);
			auto domains_ptr = domain_mgr.get(ptr->id);
			for(auto const& d : *domains_ptr) {
				ptr->domains.push_back(d);
			}
		}
	}
	catch(const std::runtime_error& e) {
		LOG(ERROR) << e.what();
		ptr.reset();
	}
	
	return std::move(ptr);
}

} // manager
} // dbl

#include "domain_manager.hxx"
#include "dbl/core/common.hxx"
#include "dbl/db/db.hxx"
#include "dbl/db/dml/queries.hxx"
#include "domain_list_manager.hxx"


namespace dbl {
namespace manager {

DomainManager::DomainManager(std::shared_ptr<core::Api> api)
	: Manager(api)
{
}

std::unique_ptr<types::DomainSet_t>
DomainManager::get(int list_id)
{
	using namespace soci;
	using types::DomainSet_t;

	std::unique_ptr<DomainSet_t> ptr(new DomainSet_t());
	try {
		auto session_ptr = api_->db()->session();

		Domain record;
		std::string q(dml::queries::get_domains);

		if(list_id) {
			q.append(" WHERE list_id = ");
			q.append(std::to_string(list_id));
		}

		q.append(" ORDER BY d.name asc");
		statement st = (session_ptr->prepare << q, into(record));

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

std::unique_ptr<types::Domain>
DomainManager::get(const std::string& name)
{
	using namespace soci;
	using types::Domain;

	std::unique_ptr<Domain> ptr(new Domain());

	std::string q(dml::queries::get_domains + "  WHERE d.name = ?");
	try {
		auto session_ptr = api_->db()->session();
		*session_ptr << q, use(name), into(*ptr);
	}
	catch(const std::runtime_error& e) {
		LOG(ERROR) << e.what();
		ptr.reset();
	}

	return std::move(ptr);
}

std::unique_ptr<types::DomainSet_t>
DomainManager::get_blocked()
{
	using namespace soci;
	using types::DomainSet_t;
	using types::Domain;

	std::unique_ptr<DomainSet_t> ptr(new DomainSet_t());

	Domain record;
	try {
		auto session_ptr = api_->db()->session();

		statement st = (
			session_ptr->prepare << dml::queries::get_blocked_domains,
			into(record)
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

std::unique_ptr<types::DomainSet_t>
DomainManager::get_whitelisted()
{
	using namespace soci;
	using types::DomainSet_t;

	std::unique_ptr<DomainSet_t> ptr(new DomainSet_t());

	try {
		auto session_ptr = api_->db()->session();
		Domain record;

		statement st = (
			session_ptr->prepare << dml::queries::get_whitelisted_domains,
			into(record)
		);

		st.execute(true);
		while(st.fetch()) {
			ptr->push_back(record);
		}
	}
	catch(const std::runtime_error& e) {
		LOG(ERROR) << e.what();
	}

	return std::move(ptr);
}

void DomainManager::block_domains(const types::Names_t& domains,
								  int list_id)
{
	using namespace soci;

	if(domains.empty()) {
		return;
	}

	if(!list_id) {
		DomainListManager list_mgr(api_);
		list_id = list_mgr.create("CUSTOM", "Custom list", true);
	}

	try {
		auto session_ptr = api_->db()->session();
		session_ptr->begin();

		statement st_del = (
			session_ptr->prepare << "DELETE FROM whitelisted_domains "
			<< "WHERE name = ?",
			use(domains)
		);

		st_del.execute(true);

		std::string q(
			"INSERT OR REPLACE INTO domains(name, list_id)"
			"  VALUES(?, " + std::to_string(list_id) + ")"
		);

		statement st_ins = (session_ptr->prepare << q, use(domains));
		st_ins.execute(true);
		session_ptr->commit();
	}
	catch(const std::runtime_error& e) {
		LOG(ERROR) << e.what();
	}
}

void DomainManager::unblock_domains(const types::Names_t& domains)
{
	using namespace soci;

	try {
		auto session_ptr = api_->db()->session();
		session_ptr->begin();

		std::string q(
			"INSERT OR IGNORE INTO whitelisted_domains(name)"
			"  VALUES(?)"
		);

		statement st = (session_ptr->prepare << q, use(domains));
		st.execute(true);
		session_ptr->commit();
	}
	catch(const std::runtime_error& e) {
		LOG(ERROR) << e.what();
	}
}

} // manager
} // dbl

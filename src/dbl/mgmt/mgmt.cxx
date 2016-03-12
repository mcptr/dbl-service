#include "mgmt.hxx"
#include "dbl/manager/domain_manager.hxx"
#include "dbl/manager/domain_list_manager.hxx"

namespace dbl {
namespace mgmt {

Mgmt::Mgmt(std::shared_ptr<core::Api> api,
		   const Options po)
	: api_(api),
	  po_(po),
	  add_list_(po.get<bool>("add-list")),
	  delete_list_(po.get<std::string>("delete-list")),
	  export_list_(po.get<std::string>("export-list")),
	  block_domains_(po.get<std::vector<std::string>>("block")),
	  unblock_domains_(po.get<std::vector<std::string>>("unblock"))
{
}

bool Mgmt::has_work() const
{
	return (
		add_list_ ||
		!delete_list_.empty() ||
		!export_list_.empty() ||
		!block_domains_.empty() ||
		!unblock_domains_.empty()
	);
}

bool Mgmt::manage()
{
	if(add_list_) {
		return add_list();
	}
	else if(!delete_list_.empty()) {
		return delete_list();
	}
	else if(!block_domains_.empty() || ! unblock_domains_.empty()) {
		return manage_domains();
	}
	else if(!export_list_.empty()) {
		return export_list();
	}

	return false;
}

bool Mgmt::manage_domains()
{	
	dbl::manager::DomainManager mgr(api_);

	if(!unblock_domains_.empty()) {
		mgr.unblock_domains(unblock_domains_);
	}

	if(!block_domains_.empty()) {
		for(auto const& domain : block_domains_) {
			// slow...
			bool found = std::find(
				unblock_domains_.begin(),
				unblock_domains_.end(),
				domain
			) != unblock_domains_.end();

			if(found) {
				std::string msg(
					"Conflicting domain (will be blocked): "
				);
				msg.append(domain);
				if(api_->config.is_fatal) {
					throw std::runtime_error(msg);
				}
				std::cerr << "WARNING: " << msg 
						  << std::endl;
				LOG(WARNING) << msg;
			}
		}

		mgr.block_domains(block_domains_);
	}

	return true;
}

bool Mgmt::export_list()
{
	dbl::manager::DomainListManager mgr(api_);
	auto dl = mgr.get(export_list_ , true);
	if(dl) {
		std::cout << dl->to_json_string() << std::endl;
	}

	return true;
}

bool Mgmt::add_list()
{
	const std::string name = po_.get<std::string>("list-name");
	const std::string url = po_.get<std::string>("list-url");
	const std::string description = po_.get<std::string>("list-description");

	if(name.empty()) {
		throw std::runtime_error("Cannot add list, name missing");
	}

	if(url.empty()) {
		throw std::runtime_error("Cannot add list, url missing");
	}

	dbl::manager::DomainListManager mgr(api_);
	return (mgr.create(name, url, description, false) > 0);
}

bool Mgmt::delete_list()
{
	return false;
}

} // mgmt
} // dbl

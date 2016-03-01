#include "query.hxx"
#include "dbl/manager/domain_manager.hxx"
#include "dbl/manager/domain_list_manager.hxx"

#include <iostream>

namespace dbl {
namespace query {

Query::Query(const Options& po, std::shared_ptr<core::Api> api)
	: po_(po),
	  api_(api)
{
}

bool Query::run()
{
	if(po_.get<bool>("domain-lists")) {
		print_domain_lists();
	}
	else if(po_.get<bool>("domains")) {
		print_blocked_domains();
	}
	else if(po_.get<std::string>("domain").length()) {
		return print_domain_details(po_.get<std::string>("domain"));
	}

	return true;
}

void Query::print_domain_lists()
{
	manager::DomainListManager mgr(api_);
	auto records_ptr = mgr.get();
	for(auto const& r : *records_ptr) {
		std::cout << r.name << " :: "
				  << "Active: " << r.active << ", Custom: " << r.custom
				  << " :: " << r.description
				  << std::endl;
	}
}

void Query::print_blocked_domains()
{
	manager::DomainManager mgr(api_);
	auto ptr = mgr.get_blocked();
	for(auto const& r : *ptr) {
		std::cout << r.name;
		if(api_->config.is_verbose) {
			if(r.description.is_initialized()) {
				std::cout << " - " << r.description.get();
			}
		}

		std::cout << std::endl;
	}
}

bool Query::print_domain_details(const std::string& domain)
{
	manager::DomainManager mgr(api_);
	auto ptr = mgr.get(domain);
	if(ptr->id) {
		std::cout << ptr->name << "\n"
				  << "List: " << ptr->list_id 
				  << ", " << ptr->list_name << "\n"
				  << ptr->description
				  << std::endl;
		return true;
	}

	return false;
}

} // query
} // dbl

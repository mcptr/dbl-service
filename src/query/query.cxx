#include "query.hxx"

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
	auto records_ptr = api_->db()->get_domain_lists();
	for(auto const& r : *records_ptr) {
		std::cout << r.name << " :: "
				  << "Active: " << r.active << ", Custom: " << r.custom
				  << " :: " << r.description
				  << std::endl;
	}
}

void Query::print_blocked_domains()
{
	auto ptr = api_->db()->get_blocked_domains();
	for(auto const& r : *ptr) {
		std::cout << r.name;
		if(api_->config.is_verbose) {
			if(r.description.is_initialized()
			   && !r.description->empty())
			{
				std::cout << " - " << r.description;
			}
		}

		std::cout << std::endl;
	}
}

bool Query::print_domain_details(const std::string& domain)
{
	auto ptr = api_->db()->get_domain(domain);
	if(ptr->id) {
		std::cout << ptr->name << "\n"
				  << "Active: " << ptr->active << ", "
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

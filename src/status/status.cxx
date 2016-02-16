#include "status.hxx"
#include <iostream>

namespace dbl {

Status::Status(std::shared_ptr<RTApi> api)
	: api_(api)
{
}

void Status::print_lists()
{
	auto records_ptr = api_->db()->get_domain_lists();
	for(auto const& r : *records_ptr) {
		std::cout << (r.active ? "+" : "-") << (r.custom ? "@ " : "  ")
					  << r.name << " - " << r.description
					  << std::endl;
	}
}

void Status::print_domains(bool active_only)
{
	auto records_ptr = api_->db()->get_domains(active_only);
	for(auto const& r : *records_ptr) {
		std::cout << r.name << " - " << r.description << std::endl;
	}
}

} // dbl

#include "mgmt.hxx"
#include "dbl/manager/domain_manager.hxx"
#include "dbl/manager/domain_list_manager.hxx"

namespace dbl {
namespace mgmt {

void manage_domains(std::shared_ptr<dbl::core::Api> api,
					std::vector<std::string> block,
					std::vector<std::string> unblock)
{	
	dbl::manager::DomainManager mgr(api);

	if(!unblock.empty()) {
		mgr.unblock_domains(unblock);
	}

	if(!block.empty()) {
		for(auto const& domain : block) {
			// slow...
			bool found = std::find(
				unblock.begin(),
				unblock.end(),
				domain
			) != unblock.end();

			if(found) {
				std::string msg(
					"Conflicting domain (will be blocked): "
				);
				msg.append(domain);
				if(api->config.is_fatal) {
					throw std::runtime_error(msg);
				}
				std::cerr << "WARNING: " << msg 
						  << std::endl;
				LOG(WARNING) << msg;
			}
		}

		mgr.block_domains(block);
	}
}

void manage_import_export(std::shared_ptr<dbl::core::Api> api,
						  std::vector<std::string> lst)
{
	if(lst.size()) {
		dbl::manager::DomainListManager mgr(api);
		for(auto const& name : lst) {
			auto dl = mgr.get(name, true);
			if(dl) {
				std::cout << "\nList: " << name << std::endl;
				std::cout << dl->to_json_string() << std::endl;
			}
		}
	}
}

} // mgmt
} // dbl

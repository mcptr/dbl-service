#ifndef DBL_MGMT_MGMT_HXX
#define DBL_MGMT_MGMT_HXX

#include "dbl/core/api.hxx"

#include <string>
#include <vector>
#include <memory>

namespace dbl {
namespace mgmt {

void manage_domains(std::shared_ptr<dbl::core::Api> api,
					std::vector<std::string> block,
					std::vector<std::string> unblock);

void manage_import_export(std::shared_ptr<dbl::core::Api> api,
						  std::vector<std::string> lst);

} // mgmt
} // dbl

#endif

#ifndef DBL_LIST_LIST_HXX
#define DBL_LIST_LIST_HXX

#include "db/types/types.hxx"

#include <vector>

namespace dbl {
namespace list {

class List : public db::types::DomainList
{
public:

	List() = default;
	explicit List(const std::string& lst_name);
	virtual ~List() = default;

	bool from_json(const std::string& input);
private:
};

} // list
} // dbl

#endif

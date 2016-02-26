#ifndef DBL_LIST_LIST_HXX
#define DBL_LIST_LIST_HXX

#include "entry.hxx"
#include <unordered_map>

namespace dbl {
namespace list {

class List
{
public:
	typedef std::unordered_map<std::string, Entry> Entries_t;

	explicit List(const std::string& name);
	~List() = default;

	inline const std::string& get_name() const
	{
		return name_;
	}

	inline const Entries_t& get_domains() const
	{
		return domains_;
	}

	void add_form_json(const std::string& input);
	void add_from_text(const std::string& input);
	void add_from_file(const std::string& path);
private:
	const std::string name_ = "";
	std::unordered_map<std::string, Entry> domains_;
};

} // list
} // dbl

#endif

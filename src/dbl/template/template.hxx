#ifndef DBL_TEMPLATE_HXX
#define DBL_TEMPLATE_HXX

#include <string>
#include <unordered_map>

namespace dbl {

class Template
{
public:
	typedef std::unordered_map<std::string, std::string> Values_t;

	void load(const std::string& root, const std::string& name);
	const std::string& render(const Values_t& values);
	
private:
	std::string content_;
};

} // dbl

#endif

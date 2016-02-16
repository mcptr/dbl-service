#include "template.hxx"
#include "core/common.hxx"

#include <fstream>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace dbl {

void Template::load(const std::string& root, const std::string& name)
{
	namespace fs = boost::filesystem;
	fs::path path(root);
	path /= name;

	if(!fs::exists(path)) {
		throw std::runtime_error("Template not found: " + path.string());
	}

	std::ifstream fh(path.string());
	content_.assign(
		(std::istreambuf_iterator<char>(fh)),
		std::istreambuf_iterator<char>()
	);
	fh.close();
}

const std::string& Template::render(const Values_t& values)
{
	for(auto const& it : values) {
		std::string k = "$" + it.first;
		boost::replace_all(content_, k, it.second);
	}

	return content_;
}

} // dbl

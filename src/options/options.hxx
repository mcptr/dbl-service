#ifndef DBL_OPTIONS_OPTIONS_HXX
#define DBL_OPTIONS_OPTIONS_HXX

#include "config/config.hxx"
#include <boost/program_options.hpp>
#include <string>

namespace dbl {

namespace po = boost::program_options;

class Options
{
public:
	Options() = default;
	virtual ~Options() = default;

	void parse(int argc, char** argv, config::Config& config);
	bool has_help() const;
	void display_help() const;

	template <class T>
	const T get(const std::string& k) const
	{
		try {
			return vm_[k].as<T>();
		}
		catch(boost::bad_any_cast& e) {
			std::cerr << "ERROR: Options::get(): " << k << std::endl;
			throw e;
		}
	}

	void dump_variables_map() const;
	void throw_on_conflict(const std::string& x, const std::string& y);

protected:
	po::variables_map vm_;
	po::options_description all_;
};

} // dbl

#endif

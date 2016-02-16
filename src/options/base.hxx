#ifndef DBL_OPTIONS_BASE_HXX
#define DBL_OPTIONS_BASE_HXX


#include <boost/program_options.hpp>
#include <string>

namespace dbl {

namespace po = boost::program_options;

class OptionsBase
{
public:
	OptionsBase() = default;
	virtual ~OptionsBase() = default;

	void parse(int argc, char** argv);
	bool has_help() const;
	void display_help() const

	template <class T>
	const T get(const std::string& k) const
	{
		try {
			return vm_[k].as<T>();
		}
		catch(boost::bad_any_cast& e) {
			std::cerr << "ERROR: OptionsBase::get(): " << k << std::endl;
			throw e;
		}
	}

	void dump_variables_map() const;

protected:
	po::variables_map vm_;
	po::options_description all_;
};

} // dbl

#endif

#ifndef DBL_DB_UTILS_HXX
#define DBL_DB_UTILS_HXX

#include <string>
#include <boost/optional.hpp>
#define SOCI_USE_BOOST
#include <boost/tuple/tuple.hpp>
#include <soci/soci.h>

namespace dbl {
namespace db {
namespace utils {

template<class T>
void set_optional_value(soci::values& v,
						const std::string& k,
						const boost::optional<T>& opt)
{
	v.set(
		k,
		opt.get(),
		(opt.is_initialized() ? soci::i_ok : soci::i_null)
	);
}

template<class T>
T get_value(const boost::optional<T>& opt)
{
	return (opt.is_initialized() ? *opt : T());
}

template<class T>
soci::indicator get_value_indicator(const boost::optional<T>& opt,
									bool null_on_empty = false)
{
	soci::indicator ind = (opt.is_initialized() ? soci::i_ok : soci::i_null);
	T value = get_value(opt);
	return (null_on_empty && !value ? soci::i_null : ind);
}

template<>
soci::indicator get_value_indicator(const boost::optional<std::string>& opt,
									bool null_on_empty);

template<>
soci::indicator get_value_indicator(const boost::optional<std::tm>& opt,
									bool null_on_empty);

} // utils
} // db
} // dbl

#endif


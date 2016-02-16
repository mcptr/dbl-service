#include "utils.hxx"
#include <ctime>

namespace dbl {
namespace dbutils {

template<>
soci::indicator get_value_indicator(const boost::optional<std::string>& opt,
									bool null_on_empty)
{
	soci::indicator ind = (opt.is_initialized() ? soci::i_ok : soci::i_null);
	std::string value = get_value(opt);
	return (null_on_empty && value.empty() ? soci::i_null: ind);
}

template<>
soci::indicator get_value_indicator(const boost::optional<std::tm>& opt,
									bool null_on_empty)
{
	soci::indicator ind = (opt.is_initialized() ? soci::i_ok : soci::i_null);
	if(null_on_empty) {
		std::tm value = get_value(opt);
		std::time_t t = std::mktime(&value);
		if(t == 0) {
			return soci::i_null;
		}
	}
	return ind;
}

} // dbutils
} // dbl

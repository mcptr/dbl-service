#include "base.hxx"

namespace dbl {

void BaseConfig::set_bool_value(const std::string& name, bool value)
{
	bool_values_[name] = value;
}

void BaseConfig::set_int_value(const std::string& name, int value)
{
	int_values_[name] = value;
}

void BaseConfig::set_string_value(const std::string& name,
							   const std::string& value)
{
	string_values_[name] = value;
}

bool BaseConfig::get_bool_value(const std::string& name,
								bool default_value) const
{
	BoolValues_t::const_iterator it = bool_values_.find(name);
	return (it != bool_values_.end() ? it->second : default_value);
}

int BaseConfig::get_int_value(const std::string& name,
							  int default_value) const
{
	IntValues_t::const_iterator it = int_values_.find(name);
	return (it != int_values_.end() ? it->second : default_value);
}

std::string BaseConfig::get_string_value(const std::string& name,
										 const std::string& default_value) const
{
	StringValues_t::const_iterator it = string_values_.find(name);
	return (it != string_values_.end() ? it->second : default_value);
}

} // dbl

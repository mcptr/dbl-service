#ifndef DBL_CONFIG_BASE_HXX
#define DBL_CONFIG_BASE_HXX

#include <string>
#include <unordered_map>


namespace dbl {

class BaseConfig
{
public:
	typedef std::unordered_map<std::string, bool> BoolValues_t;
	typedef std::unordered_map<std::string, int> IntValues_t;
	typedef std::unordered_map<std::string, std::string> StringValues_t;
	typedef std::unordered_map<std::string, std::string> Dirs_t;

	BaseConfig() = default;
	BaseConfig(const BaseConfig&) = default;
	virtual ~BaseConfig() = default;

	virtual void set_bool_value(const std::string& name, bool value) final;
	virtual void set_int_value(const std::string& name, int value) final;

	virtual void set_string_value(
		const std::string& name,
		const std::string& value) final;

	virtual bool get_bool_value(const std::string& name,
								bool default_value = false) const final;

	virtual int get_int_value(const std::string& name,
							  int default_value = 0) const final;

	virtual std::string	get_string_value(
		const std::string& name,
		const std::string& default_value = std::string()) const final;

protected:
	IntValues_t int_values_;
	BoolValues_t bool_values_;
	StringValues_t string_values_;
};

} // dbl


#endif

#ifndef DBL_LIST_LIST_HXX
#define DBL_LIST_LIST_HXX

#include "entry.hxx"
#include <unordered_map>

namespace dbl {
namespace list {

List::List(const std::string& name)
	: name_(name)
{
}

void List::add_from_json(const std::string& input)
{
}

void List::add_from_text(const std::string& input)
{
}

void List::add_from_file(const std::string& path)
{
}

} // list
} // dbl

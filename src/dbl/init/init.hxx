#ifndef DBL_INIT_INIT_HXX
#define DBL_INIT_INIT_HXX

#include "dbl/options/options.hxx"
#include "dbl/core/common.hxx"

namespace dbl {
namespace init {


void setup_logging(const dbl::Options& po);
void validate_options(const dbl::Options& po);

} // init
} // dbl

#endif

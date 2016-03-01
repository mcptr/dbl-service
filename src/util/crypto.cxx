#include "crypto.hxx"

#include <sstream>
#include <iomanip>
#include <openssl/md5.h>

namespace dbl {
namespace crypto {

std::string md5(const std::string& input)
{
	unsigned char digest[MD5_DIGEST_LENGTH];
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, input.c_str(), input.length());
    MD5_Final(digest, &ctx);

	std::stringstream result;
	for(int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		result << std::hex
			   << std::setw(2)
			   << std::setfill('0')
			   << static_cast<unsigned int>(digest[i]);
    }
	return result.str();
}

} // crypto
} // dbl

#if !defined(__BASE_UTIL_RANDOM_H__)
#define __BASE_UTIL_RANDOM_H__

#include <string>

namespace base::util {

std::string genRandomID(size_t len = 16);

} // namespace base::util

#endif // __BASE_UTIL_RANDOM_H__

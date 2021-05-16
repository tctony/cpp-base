#if !defined(__BASE_UTIL_COMMON_H__)
#define __BASE_UTIL_COMMON_H__

#define _FOR(_i, _n) for (int _i = 0; _i < _n; ++_i)
#define _RFOR(_i, _n) for (int _i = _n; _i > 0; --_i)

#define _CONCAT_IMPL(s1, s2) s1##s2
#define _CONCAT(s1, s2) __CONCAT_IMPL(s1, s2)

#ifdef __COUNTER__
// Modular builds build each module with its own preprocessor state, meaning
// `__COUNTER__` no longer provides a unique number across a TU.  Instead of
// calling back to just `__LINE__`, use a mix of `__COUNTER__` and `__LINE__`
// to try provide as much uniqueness as possible.
#define _ANONYMOUS_VARIABLE(str) __CONCAT(str, __COUNTER__)
#else
#define _ANONYMOUS_VARIABLE(str) __CONCAT(str, __LINE__)
#endif // __COUNTER__

#endif // __BASE_UTIL_COMMON_H__

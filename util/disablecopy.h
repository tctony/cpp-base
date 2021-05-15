#ifndef __BASE_UTIL_DISABLECOPY_H__
#define __BASE_UTIL_DISABLECOPY_H__

#define CLASS_DISABLE_COPY(_class)                                             \
  _class(const _class &) = delete;                                             \
  _class &operator=(const _class &) = delete;

#define CLASS_DISABLE_MOVE(_class)                                             \
  _class(_class &&) = delete;                                                  \
  _class &operator=(_class &&) = delete;

#define CLASS_DISABLE_COPY_MOVE(_class)                                        \
  _class(const _class &) = delete;                                             \
  _class &operator=(const _class &) = delete;                                  \
  _class(_class &&) = delete;                                                  \
  _class &operator=(_class &&) = delete;

#endif // __BASE_UTIL_DISABLECOPY_H__

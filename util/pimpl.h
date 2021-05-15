#if !defined(__BASE_UTIL_PIMPL__)
#define __BASE_UTIL_PIMPL__

#define PIMPL_DECLARE(CLS_NAME)                                                \
public:                                                                        \
  virtual ~CLS_NAME();                                                         \
  CLS_NAME(const CLS_NAME &o) = delete;                                        \
  CLS_NAME &operator=(const CLS_NAME &o) = delete;                             \
  CLS_NAME(CLS_NAME &&off_t);                                                  \
  CLS_NAME &operator=(CLS_NAME &&o);                                           \
                                                                               \
private:                                                                       \
  struct Impl;                                                                 \
  std::unique_ptr<Impl> m_;                                                    \
  friend struct Impl

#define PIMPL_IMPLEMENT(CLS_NAME)                                              \
  CLS_NAME::~CLS_NAME() = default;                                             \
  CLS_NAME::CLS_NAME(CLS_NAME &&o) = default;                                  \
  CLS_NAME &CLS_NAME::operator=(CLS_NAME &&o) = default

#define PIMPL_DECLARE_WITH_EMPTY_CTOR(CLS_NAME)                                \
  PIMPL_DECLARE(CLS_NAME);                                                     \
                                                                               \
public:                                                                        \
  CLS_NAME()

#define PIMPL_IMPLEMENT_WITH_EMPTY_CTOR(CLS_NAME)                              \
  PIMPL_IMPLEMENT(CLS_NAME);                                                   \
                                                                               \
  CLS_NAME::CLS_NAME() : m_(new Impl()) {}

#endif // __BASE_UTIL_PIMPL__

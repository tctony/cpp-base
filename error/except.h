#if !defined(__BSAE_ERROR_EXCEPT_H__)
#define __BSAE_ERROR_EXCEPT_H__

#include <exceptxx/BaseException.h>
#include <exceptxx/BaseExceptionImpl.h>
#include <exceptxx/ThrowHelper.h>

namespace base::error {

class Exception : public exceptxx::BaseExceptionImpl<Exception> {
public:
  using Error = std::string;

  Exception(Error error, const char *func, size_t line, std::string &&message)
      : BaseExceptionImpl(func, line, std::move(message)),
        error_(std::move(error)) {}

  const char *tag() const override;
  std::string error() const override;

private:
  Error error_;
};

} // namespace base::error

#define LIKELY(cond) __builtin_expect(!!(cond), 1)
#define UNLIKELY(cond) __builtin_expect(!!(cond), 0)

#define ENFORCE(cond)                                                          \
  if (UNLIKELY(!(cond)))                                                       \
  exceptxx::ThrowHelper<::base::error::Exception>(                             \
      #cond, EXCEPTXX_GET_FUNC(), __LINE__) <<= std::ostringstream()

#define THROW()                                                                \
  exceptxx::ThrowHelper<::base::error::Exception>(                             \
      "Throw exception", EXCEPTXX_GET_FUNC(), __LINE__) <<=                    \
      std::ostringstream()

#endif // __BSAE_ERROR_EXCEPT_H__

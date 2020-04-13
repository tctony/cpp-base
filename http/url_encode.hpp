#if !defined(BASE_HTTP_URI_ENCODE)
#define BASE_HTTP_URI_ENCODE

#include <string>

namespace base {
namespace http {
namespace detail {

template <typename CharT>
inline CharT hex_to_letter(CharT in) {
  switch (in) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      return in + '0';
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    default:
      return in - 10 + 'A';
  }
  return CharT();
}

template <typename CharT, class OutputIterator>
void encode_char(CharT in, OutputIterator &out) {
  switch (in) {
    case 'a':
    case 'A':
    case 'b':
    case 'B':
    case 'c':
    case 'C':
    case 'd':
    case 'D':
    case 'e':
    case 'E':
    case 'f':
    case 'F':
    case 'g':
    case 'G':
    case 'h':
    case 'H':
    case 'i':
    case 'I':
    case 'j':
    case 'J':
    case 'k':
    case 'K':
    case 'l':
    case 'L':
    case 'm':
    case 'M':
    case 'n':
    case 'N':
    case 'o':
    case 'O':
    case 'p':
    case 'P':
    case 'q':
    case 'Q':
    case 'r':
    case 'R':
    case 's':
    case 'S':
    case 't':
    case 'T':
    case 'u':
    case 'U':
    case 'v':
    case 'V':
    case 'w':
    case 'W':
    case 'x':
    case 'X':
    case 'y':
    case 'Y':
    case 'z':
    case 'Z':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
    case '.':
    case '_':
    case '~':
      out++ = in;
      break;
    case ' ':
      out++ = '+';
      break;
    default:
      out++ = '%';
      out++ = hex_to_letter((in >> 4) & 0x0f);
      out++ = hex_to_letter(in & 0x0f);
      break;
  }
}

}  // namespace detail

template <class InputIterator, class OutputIterator>
OutputIterator urlEncode(const InputIterator &in_begin,
                         const InputIterator &in_end,
                         const OutputIterator &out_begin) {
  InputIterator it = in_begin;
  OutputIterator out = out_begin;
  while (it != in_end) {
    detail::encode_char(*it, out);
    ++it;
  }
  return out;
}

template <class SinglePassRange, class OutputIterator>
inline OutputIterator urlEncode(const SinglePassRange &range,
                                const OutputIterator &out) {
  return urlEncode(std::begin(range), std::end(range), out);
}

inline std::string urlEncode(const std::string &input) {
  std::string encoded;
  urlEncode(input, std::back_inserter(encoded));
  return encoded;
}

}  // namespace http
}  // namespace base

#endif  // BASE_HTTP_URI_ENCODE

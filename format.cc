

#include "format.h"

#include <cassert>
#include <cstring>
#include <algorithm>

using std::size_t;

template <typename T>
void fmt::Formatter::FormatArg(
    const char *format, const T &arg, int width, int precision) {
  size_t offset = buffer_.size();
  buffer_.resize(buffer_.capacity());
  for (;;) {
    size_t size = buffer_.size() - offset;
    int n = 0;
    if (width < 0) {
      n = precision < 0 ?
          snprintf(&buffer_[offset], size, format, arg) :
          snprintf(&buffer_[offset], size, format, precision, arg);
    } else {
      n = precision < 0 ?
          snprintf(&buffer_[offset], size, format, width, arg) :
          snprintf(&buffer_[offset], size, format, width, precision, arg);
    }
    if (n >= 0 && offset + n < buffer_.size()) {
      buffer_.resize(offset + n);
      return;
    }
    buffer_.resize(n >= 0 ? offset + n + 1 : 2 * buffer_.size());
  }
}

void fmt::Formatter::Format() {
  buffer_.reserve(500);
  const char *start = format_;
  const char *s = start;
  for (; *s; ++s) {
    if (*s != '{') continue;
    buffer_.insert(buffer_.end(), start, s);
    ++s;

    // Parse argument index.
    unsigned arg_index = 0;
    if ('0' <= *s && *s <= '9') {
      do {
        arg_index = arg_index * 10 + (*s++ - '0');
      } while ('0' <= *s && *s <= '9');
    } else {
      throw FormatError("missing argument index in format string");
    }
    // TODO: check if argument index is correct

    char arg_format[8];  // longest format: %+0*.*ld
    char *arg_format_ptr = arg_format;
    *arg_format_ptr++ = '%';

    char type = 0;
    int width = -1;
    int precision = -1;
    if (*s == ':') {
      ++s;
      if (*s == '+')
        *arg_format_ptr++ = *s++;
      if (*s == '0')
        *arg_format_ptr++ = *s++;

      // Parse width.
      if ('0' <= *s && *s <= '9') {
        *arg_format_ptr++ = '*';
        width = 0;
        do {
          width = width * 10 + (*s++ - '0');
        } while ('0' <= *s && *s <= '9');
      }

      // Parse precision.
      if (*s == '.') {
        *arg_format_ptr++ = '.';
        *arg_format_ptr++ = '*';
        ++s;
        precision = 0;
        if ('0' <= *s && *s <= '9') {
          do {
            precision = precision * 10 + (*s++ - '0');
          } while ('0' <= *s && *s <= '9');
        } else {
          // TODO: error
        }
      }

      // Parse type.
      if (*s == 'f' || *s == 'g')
        type = *s++; // TODO: check if the type matches
    }

    if (*s++ != '}')
      throw FormatError("single '{' encountered in format string");
    start = s;

    // Format argument.
    Arg &arg = args_[arg_index];
    switch (arg.type) {
    case CHAR:
      if (width == -1 && precision == -1) {
        buffer_.push_back(arg.int_value);
        break;
      }
      *arg_format_ptr++ = 'c';
      *arg_format_ptr = '\0';
      FormatArg(arg_format, arg.int_value, width, precision);
      break;
    case INT:
      *arg_format_ptr++ = 'd';
      *arg_format_ptr = '\0';
      FormatArg(arg_format, arg.int_value, width, precision);
      break;
    case UINT:
      *arg_format_ptr++ = 'd';
      *arg_format_ptr = '\0';
      FormatArg(arg_format, arg.uint_value, width, precision);
      break;
    case LONG:
      *arg_format_ptr++ = 'l';
      *arg_format_ptr++ = 'd';
      *arg_format_ptr = '\0';
      FormatArg(arg_format, arg.long_value, width, precision);
      break;
    case ULONG:
      *arg_format_ptr++ = 'l';
      *arg_format_ptr++ = 'd';
      *arg_format_ptr = '\0';
      FormatArg(arg_format, arg.ulong_value, width, precision);
      break;
    case DOUBLE:
      *arg_format_ptr++ = type ? type : 'g';
      *arg_format_ptr = '\0';
      FormatArg(arg_format, arg.double_value, width, precision);
      break;
    case LONG_DOUBLE:
      *arg_format_ptr++ = 'l';
      *arg_format_ptr++ = 'g';
      *arg_format_ptr = '\0';
      FormatArg(arg_format, arg.long_double_value, width, precision);
      break;
    case STRING:
      if (width == -1 && precision == -1) {
        const char *str = arg.string_value;
        buffer_.insert(buffer_.end(), str, str + std::strlen(str));
        break;
      }
      *arg_format_ptr++ = 's';
      *arg_format_ptr = '\0';
      FormatArg(arg_format, arg.string_value, width, precision);
      break;
    case WSTRING:
      *arg_format_ptr++ = 'l';
      *arg_format_ptr++ = 's';
      *arg_format_ptr = '\0';
      FormatArg(arg_format, arg.wstring_value, width, precision);
      break;
    case POINTER:
      *arg_format_ptr++ = 'p';
      *arg_format_ptr = '\0';
      FormatArg(arg_format, arg.pointer_value, width, precision);
      break;
    default:
      assert(false);
      break;
    }
  }
  buffer_.insert(buffer_.end(), start, s + 1);
}

fmt::ArgFormatter::~ArgFormatter() {
  if (!formatter_) return;
  FinishFormatting();
}


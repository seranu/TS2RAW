#ifndef _UTILS_H_
#define _UTILS_H_

#include <exception>
#include <memory>
#include <string>

namespace ts2raw {
namespace utils {

enum class Endianness { BigEndian, LittleEndian };

// implementation of make_unique for C++11
// source: https://herbsutter.com/gotw/_102/
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// exception class used in various stages of extraction
class TSException : public std::exception {
public:
  TSException(const std::string &message) : _message(message) {}
  virtual const char *what() const throw() { return _message.c_str(); }

private:
  std::string _message;
};

// methods for reading binary data

// read an unsigend 8bit integer from <aFrom>
//   <aFrom>   is the input buffer. It must be allocated and not null,
//             with a size of at least 1 byte
uint8_t ReadUInt8(const unsigned char *aFrom);

// read an unsigned 16bit integer from <aFrom>
//   <aFrom>   is the input buffer. It must be allocated and not null,
//             with a size of at least 2 bytes
//   <aEndian> is the Endianness of the input data

uint16_t ReadUInt16(const unsigned char *aFrom, Endianness aEndian);

// read an unsigned 32bit integer from <aFrom>
//   <aFrom>   is the input buffer. It must be allocated and not null,
//             with a size of at least 4 bytes
//   <aEndian> is the Endianness of the input data
uint32_t ReadUInt32(const unsigned char *aFrom, Endianness aEndian);

} // namespace utils
} // namespace ts2raw

#endif //_UTILS_H_
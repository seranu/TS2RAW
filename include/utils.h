#ifndef _UTILS_H_
#define _UTILS_H_

#include <memory>
#include <string>
#include <exception>

namespace ts2raw {
namespace utils {
    
enum class Endianness
{
    BigEndian,
    LittleEndian
};

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class TSException: public std::exception
{
public:
    TSException(const std::string& message) : _message(message) {}
    virtual const char* what() const throw()  {
        return _message.c_str();
    }
private:
    std::string _message;
};


uint8_t ReadUInt8(const unsigned char* aFrom);
uint16_t ReadUInt16(const unsigned char* aFrom, Endianness);
uint32_t ReadUInt32(const unsigned char* aFrom, Endianness);

} // namespace utils
} // namespace ts2raw

#endif //_UTILS_H_
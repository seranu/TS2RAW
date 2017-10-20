#include "utils.h"

#include <cstdint>
#include <cassert>

namespace utils {

uint8_t ReadUInt8(const unsigned char* aFrom) {
    assert(aFrom != nullptr);
    return static_cast<uint8_t>(aFrom[0]);
}

uint16_t ReadUInt16BE(const unsigned char* aFrom) {
    uint8_t b1 = static_cast<uint8_t>(aFrom[0]);
    uint8_t b2 = static_cast<uint8_t>(aFrom[1]);
    uint16_t r = (b1 << 8);
    r = r | b2;
    return r;
}

uint32_t ReadUInt32BE(const unsigned char* aFrom) {
    uint8_t b1 = static_cast<uint8_t>(aFrom[0]);
    uint8_t b2 = static_cast<uint8_t>(aFrom[1]);
    uint8_t b3 = static_cast<uint8_t>(aFrom[2]);
    uint8_t b4 = static_cast<uint8_t>(aFrom[3]);
    uint32_t r = (b1 << 24);
    r = r | (b2 << 16);
    r = r | (b3 << 8);
    r = r | b4;
    return r;
}

uint16_t ReadUInt16(const unsigned char* aFrom, Endianness aEndian) {
    assert(aFrom != nullptr);
    switch(aEndian) {
        case Endianness::BigEndian:
            return ReadUInt16BE(aFrom);
            break;
        default:
            throw TSException("Unknown endianness");
    }
}

uint32_t ReadUInt32(const unsigned char* aFrom, Endianness aEndian) {
    assert(aFrom != nullptr);
    switch(aEndian) {
        case Endianness::BigEndian:
            return ReadUInt32BE(aFrom);
            break;
        default:
            throw TSException("Unknown endianness");
    }
}

}
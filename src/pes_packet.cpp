#include "pes_packet.h"
#include "utils.h"

using utils::Endianness;

namespace ts2raw {

pes_header_t::pes_header(const unsigned char* aInput, int size) {
    int offset = 0;
    int32_t tmp32 = utils::ReadUInt32(aInput, Endianness::BigEndian);
    offset += 4;
    packet_start_code_prefix = (tmp32 & 0xFFFFFF00) >> 8;
    stream_id = (tmp32 & 0xFF);
    PES_packet_length = utils::ReadUInt16(aInput + offset, Endianness::BigEndian);
}


PESPacket::PESPacket(unsigned char* aInput, int aSize, const pes_header_t& aHeader) :
        _payload(nullptr),
        _payloadSize(aSize),
        _header(aHeader) {
    _payload = new unsigned char[_payloadSize];
    memcpy(_payload, aInput, _payloadSize);
}

PESPacket::~PESPacket(){
    if(_payload != nullptr){
        delete _payload;
        _payload = nullptr;
    }
}

} //namespace ts2raw
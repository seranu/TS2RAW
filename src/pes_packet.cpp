#include "pes_packet.h"
#include "utils.h"

#include <cassert>
#include <cstring>

using utils::Endianness;

#define PROGRAM_STREAM_MAP              0xBC
#define PRIVATE_STREAM_1                0xBD
#define PADDING_STREAM                  0xBE
#define PRIVATE_STREAM_2                0xBF
#define VIDEO_STREAM                    0xE0
#define AUDIO_STREAM                    0xC0
#define ECM_STREAM                      0xF0
#define EMM_STREAM                      0xF1
#define DSMCC_STREAM                    0xF2
#define IEC_13522_STREAM                0xF3
#define H222_1_TYPE_A                   0xF4
#define H222_1_TYPE_B                   0xF5
#define H222_1_TYPE_C                   0xF6
#define H222_1_TYPE_D                   0xF7
#define H222_1_TYPE_E                   0xF8
#define ANCILLARY_STREAM                0xF9
#define IEC14496_1_SL_PACKETIZED_STREAM 0xFA
#define IEC14496_1_FLEX_MUX_STREAM      0xFB
#define RESERVED_DATA_STREAM_START      0xFC
#define RESERVED_DATA_STREAM_END        0xFE
#define PROGRAM_STREAM_DIRECTORY        0xFF

namespace ts2raw {

// len(packet_start_code_prefix) = 24 
// len(stream_id) = 8
// len(PES_packet_length) = 16
const int KBaseHeaderLen = 6; 

const int PESPacket::KPESPacketMagic = 0x01;

pes_header_t::pes_header(const unsigned char* aInput, int size) {
    assert(size >= 6);
    int offset = 0;
    int32_t tmp32 = utils::ReadUInt32(aInput, Endianness::BigEndian);
    offset += 4;
    packet_start_code_prefix = (tmp32 & 0xFFFFFF00) >> 8;
    stream_id = (tmp32 & 0xFF);
    PES_packet_length = utils::ReadUInt16(aInput + offset, Endianness::BigEndian);
}

pes_extended_header_t::pes_extended_header(const unsigned char* aInput, int aSize) :
        flags(0),
        PES_header_data_length(0) {
    assert(aSize >= KBaseHeaderLen + 3);
    int offset = KBaseHeaderLen;
    flags = utils::ReadUInt16(aInput + offset, Endianness::BigEndian);
    offset += 2;
    PES_header_data_length = utils::ReadUInt8(aInput + offset); 
}


PESPacket::PESPacket(unsigned char* aInput, int aSize, const pes_header_t& aHeader) :
        _data(nullptr),
        _payload(nullptr),
        _dataSize(aSize),
        _payloadSize(0),
        _baseHeader(aHeader) {
    _data = new unsigned char[_dataSize];
    memcpy(_data, aInput, _dataSize);

    assert(_baseHeader.packet_start_code_prefix == KPESPacketMagic);

    if(_HasExtendedHeader(_baseHeader.stream_id)) {
        _extendedHeader = pes_extended_header_t(_data + KBaseHeaderLen, _dataSize - KBaseHeaderLen);
        _payload = _data + 
            KBaseHeaderLen +
            3 + // << flags + PES_header_data_length
            _extendedHeader.PES_header_data_length;

        _payloadSize = _dataSize - 
            KBaseHeaderLen - 
            3 - 
            _extendedHeader.PES_header_data_length;
            
    } else {
        _payload = _data + KBaseHeaderLen;
        _payloadSize = _dataSize - KBaseHeaderLen;
    }
}

bool PESPacket::_HasExtendedHeader(int stream_id) {
    return ((stream_id != PROGRAM_STREAM_MAP) &&
        (stream_id != PADDING_STREAM) &&
        (stream_id != PRIVATE_STREAM_2) &&
        (stream_id != ECM_STREAM) &&
        (stream_id != EMM_STREAM) &&
        (stream_id != PROGRAM_STREAM_DIRECTORY) &&
        (stream_id != DSMCC_STREAM) &&
        (stream_id != H222_1_TYPE_E));
}

PESPacket::~PESPacket(){
    if(_data != nullptr){
        delete _data;
        _data = nullptr;
    }
}

bool PESPacket::IsPESPacket(const unsigned char* aInput, int aSize) {
    if(aSize < KBaseHeaderLen) {
        return false;
    } else {
        return pes_header_t(aInput, aSize).packet_start_code_prefix == KPESPacketMagic;
    }
}

bool IsVideoStream(int aStreamId) {
    return aStreamId == VIDEO_STREAM;
}

bool IsAudioStream(int aStreamId) {
    return aStreamId == AUDIO_STREAM;
}

} //namespace ts2raw
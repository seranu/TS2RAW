#ifndef _PES_PACKET_H_
#define _PES_PACKET_H_

#include <cstdint>

namespace ts2raw {

typedef struct pes_header {
    uint32_t packet_start_code_prefix;
    uint8_t stream_id;
    uint16_t PES_packet_length;
    pes_header() : 
        packet_start_code_prefix(0), 
        stream_id(0), 
        PES_packet_length(0) {}
    pes_header(const unsigned char* aInput, int aSize);
} pes_header_t;

typedef struct pes_extended_header {
    int16_t flags;
    int8_t PES_header_data_length;
    pes_extended_header(const unsigned char* aInput, int aSize);
    pes_extended_header() : 
        flags(0),
        PES_header_data_length(0) {}
} pes_extended_header_t;

class PESPacket {
public:
    PESPacket(unsigned char* aInput, int aSize, const pes_header_t& aHeader);
    ~PESPacket();

    const unsigned char* GetPayload(int& size) {
        size = _payloadSize;
        return _payload;
    }

private:
    unsigned char* _data;
    unsigned char* _payload;
    int _dataSize;
    int _payloadSize;
    pes_header_t _baseHeader;
    pes_extended_header_t _extendedHeader;
    
    bool _HasExtendedHeader(int stream_id);
};

}


#endif //_PES_PACKET_H_
#ifndef _PES_PACKET_H_
#define _PES_PACKET_H_

#include <cstdint>

namespace ts2raw {

typedef struct pes_header {
    uint32_t packet_start_code_prefix;
    uint8_t stream_id;
    uint16_t PES_packet_length;
    pes_header() : packet_start_code_prefix(0), stream_id(0), PES_packet_length(0) {}
    pes_header(const unsigned char* aInput, int size);
} pes_header_t;

class PESPacket {
public:
    PESPacket(unsigned char* aInput, int aSize, const pes_header_t& aHeader);
    ~PESPacket();

    const unsigned char* GetPayload(int& size) {
        size = _payloadSize;
        return _payload;
    }

private:
    unsigned char* _payload;
    int _payloadSize;
    pes_header_t _header;
};

}


#endif //_PES_PACKET_H_
#ifndef _TS_PACKET_H_
#define _TS_PACKET_H_

#include <cstdint>

namespace ts2raw {

const int KStreamHeaderSize = 4;
const int KStreamPacketSize = 188;

typedef struct stream_packet_header {
	uint8_t sync_byte;
	bool transport_error_indicator;
	bool payload_unit_start_indicator;
	bool transport_priority;
	uint16_t pid;
	uint8_t transport_scrambling_control;
	uint8_t adaptation_field_control;
	uint8_t continuity_counter;
	stream_packet_header(int32_t);
} stream_packet_header_t; 

class TSPacket {

public:
    TSPacket(unsigned char* aInput, int size);

    TSPacket(const TSPacket&) = delete;
    TSPacket& operator=(const TSPacket&) = delete;

    uint16_t GetPid() const {
        return _header.pid;
    }

    bool IsPayloadUnitStartIndicatorSet() const {
        return _header.payload_unit_start_indicator;
    }

    const unsigned char* GetPayload(int& size) const {
        size = _payloadSize;
        return _payload;
    }
private:
    unsigned char _data[KStreamPacketSize];
    unsigned char* _payload;
    int _payloadSize;
    stream_packet_header_t _header;
};

}

#endif //_TS_PACKET_H_
#ifndef _TS_PACKET_H_
#define _TS_PACKET_H_

#include <cstdint>

namespace ts2raw {

// TS Packet header size
const int KStreamHeaderSize = 4;

// TS Packet standard size
const int KStreamPacketSize = 188;

// struct that describes a MPEG Transport Stream packet header
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

// class used to describe a MPEG Transport Stream packet
class TSPacket {

public:
  // constructor
  //   <aInput> input buffer
  //   <aSize>  input buffer size
  TSPacket(unsigned char *aInput, int aSize);

  TSPacket(const TSPacket &) = delete;
  TSPacket &operator=(const TSPacket &) = delete;

  // return the PID of the TS packet
  uint16_t GetPid() const { return _header.pid; }

  // returns true when a new packet starts within the payload
  // of this TS packet
  bool IsPayloadUnitStartIndicatorSet() const {
    return _header.payload_unit_start_indicator;
  }

  // returns a pointer to the payload data
  //   <aSize> size of the payload data; output parameter
  const unsigned char *GetPayload(int &aSize) const {
    aSize = _payloadSize;
    return _payload;
  }

private:
  unsigned char _data[KStreamPacketSize];
  unsigned char *_payload;
  int _payloadSize;
  stream_packet_header_t _header;
};

} // namespace ts2raw

#endif //_TS_PACKET_H_
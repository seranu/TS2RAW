#ifndef _PES_PACKET_H_
#define _PES_PACKET_H_

#include <cstdint>

namespace ts2raw {

// struct for storing PES Packet base header
typedef struct pes_header {
  uint32_t packet_start_code_prefix;
  uint8_t stream_id;
  uint16_t PES_packet_length;
  pes_header()
      : packet_start_code_prefix(0), stream_id(0), PES_packet_length(0) {}
  pes_header(const unsigned char* aInput, int aSize);
} pes_header_t;

// struct for storing some of PES Packet extended header fields
typedef struct pes_extended_header {
  uint16_t flags;
  uint8_t PES_header_data_length;
  pes_extended_header(const unsigned char* aInput, int aSize);
  pes_extended_header() : flags(0), PES_header_data_length(0) {}
} pes_extended_header_t;

// class representing a PES Packet
class PESPacket {
 public:
  // constuctor
  //   <aInput>  input buffer
  //   <aSize>   size of input buffer
  //   <aHeader> basic header of the PES packet, which is read by PESStream
  PESPacket(unsigned char* aInput, int aSize, const pes_header_t& aHeader);

  PESPacket(const PESPacket&) = delete;
  PESPacket& operator=(const PESPacket&) = delete;

  ~PESPacket();

  // returns pointer to payload data
  //   <aSize> size of the payload data; output parameter
  const unsigned char* GetPayload(int& aSize) {
    aSize = _payloadSize;
    return _payload;
  }

  // return true if <aInput> is a buffer starting with a PES packet
  //   <aInput> input buffer
  //   <aSize>  size of the input buffer
  static bool IsPESPacket(const unsigned char* aInput, int aSize);

  // constant denoting the magic number inside the PES Packet header
  static const int KPESPacketMagic;

 private:
  unsigned char* _data;
  unsigned char* _payload;
  int _dataSize;
  int _payloadSize;
  pes_header_t _baseHeader;
  pes_extended_header_t _extendedHeader;

  // returns true if <aStreamId> denotes a stream
  // for which PES packets have extended header fields
  //   <aStreamId> input stream id
  bool _HasExtendedHeader(int aStreamId);
};

// returns true if aStreamId denotes a video stream
bool IsVideoStream(int aStreamId);

// returns true if aStreamId denotes an audio stream
bool IsAudioStream(int aStreamId);

}  // namespace ts2raw

#endif  //_PES_PACKET_H_
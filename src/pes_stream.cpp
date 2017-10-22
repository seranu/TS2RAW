#include "pes_stream.h"
#include "utils.h"

#include <cassert>
#include <cstring>
#include <fstream>

using ts2raw::utils::Endianness;
using ts2raw::utils::TSException;

#define PES_PACKET_MAGIC 0x01

namespace ts2raw {

const int KMaxBufferSize = 1024 * 1024 * 10;  // << 10 MB

void PESStream::AddTSPacket(TSPacket& aTsPacket) {
  // first packet in the stream
  // set the PID of the stream
  if (_pid == -1) {
    _pid = aTsPacket.GetPid();
  }

  int tsPayloadSize = 0;
  const unsigned char* pTsPayload = aTsPacket.GetPayload(tsPayloadSize);

  if (aTsPacket.IsPayloadUnitStartIndicatorSet()) {
    // this is a new PES Packet
    // create PES packet from previously gathered data, if any
    _WrapUpLastPacket();

    // read PES packet base header
    _pCurrentHeader = new pes_header_t(pTsPayload, tsPayloadSize);

    if (_streamId == -1) {
      // set stream id if it's first packet of the stream
      _streamId = _pCurrentHeader->stream_id;
    }

    // sanity check for PES magic
    if (_pCurrentHeader->packet_start_code_prefix !=
        PESPacket::KPESPacketMagic) {
      throw TSException("Inavlid PES packet");
    }

    _bufferSize = 0;
    if (_pCurrentHeader->PES_packet_length > 0) {
      // PES packet length is specified
      _bufferSize =
          6                                      /* base header size */
          + _pCurrentHeader->PES_packet_length;  // << PES_packet_length is the
                                                 // remainder of the length of
                                                 // PES packet
    } else {
      // PES packet length is unspecified
      // allocate "some" memory
      // TODO: what is proper size to allocate here?
      _bufferSize = KMaxBufferSize;
    }
    _pCurrentData = new unsigned char[_bufferSize];

    assert(_bufferSize >= tsPayloadSize);

    // copy TS payload into the buffer
    memcpy(_pCurrentData, pTsPayload, tsPayloadSize);
    _currentOffset = tsPayloadSize;
  } else {
    // we already started a PES packet
    // only append data
    assert(_bufferSize >= _currentOffset + tsPayloadSize);
    memcpy(_pCurrentData + _currentOffset, pTsPayload, tsPayloadSize);
    _currentOffset += tsPayloadSize;
  }
}

void PESStream::_WrapUpLastPacket() {
  if (_pCurrentData != nullptr) {
    // create PES packet from gathered data
    _packets.push_back(ts2raw::utils::make_unique<PESPacket>(
        _pCurrentData, _currentOffset, *_pCurrentHeader));

    // clean-up internal buffers
    delete[] _pCurrentData;
    delete[] _pCurrentHeader;
    _pCurrentData = nullptr;
    _pCurrentHeader = nullptr;
    _currentOffset = 0;
  }
}

void PESStream::Unpack(const std::string& aOutputFilename) {
  // we might have some gathered data
  _WrapUpLastPacket();

  std::ofstream out(aOutputFilename.c_str(), std::ios::out | std::ios::binary);
  if (out.is_open()) {
    for (auto& pPacket : _packets) {
      // write payload to file
      int payloadSize = 0;
      const unsigned char* payload = pPacket->GetPayload(payloadSize);
      out.write(reinterpret_cast<const char*>(payload), payloadSize);
    }
  } else {
    throw TSException("Cannot open output file: " + aOutputFilename);
  }
}

bool PESStream::IsVideo() const {
  return IsVideoStream(_streamId);
}

bool PESStream::IsAudio() const {
  return IsAudioStream(_streamId);
}

}  // namespace ts2raw
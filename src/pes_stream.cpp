#include "pes_stream.h"
#include "utils.h"

#include <fstream>
#include <cassert>
#include <cstring>

// TODO remove
#include <iostream>

using ts2raw::utils::Endianness;
using ts2raw::utils::TSException;

#define PES_PACKET_MAGIC 0x01

namespace ts2raw {

const int KMaxBufferSize = 1024 * 1024 * 10; // << 10 MB


void PESStream::AddTSPacket(TSPacket& aTsPacket) {
    // is it first packet?
    if(_pid == -1) {
        _pid = aTsPacket.GetPid();
    }

    int tsPayloadSize = 0;
    const unsigned char* pTsPayload = aTsPacket.GetPayload(tsPayloadSize);

    if(_pid == 34){
        int a = 34;
        a = tsPayloadSize + a;
    }

    if(aTsPacket.IsPayloadUnitStartIndicatorSet()) {
        // new packet
        // anything already here?
        _WrapUpLastPacket();

        // read pes header
        _pCurrentHeader = new pes_header_t(pTsPayload, tsPayloadSize);

        if(_streamId == -1) {
            _streamId = _pCurrentHeader->stream_id;
        }

        if(_pCurrentHeader->packet_start_code_prefix != PESPacket::KPESPacketMagic) {
            throw TSException("Inavlid PES packet");
        }

        // TODO: what is pes package size here?
        _bufferSize = 0;
        if(_pCurrentHeader->PES_packet_length > 0){
            _bufferSize = 6 /* base header size */
             + _pCurrentHeader->PES_packet_length; // << PES_packet_length is the remainder of the length of PES packet
        } else {
            _bufferSize = KMaxBufferSize;
        }
        _pCurrentData = new unsigned char[_bufferSize];
        assert(_bufferSize >= tsPayloadSize);
        memcpy(_pCurrentData, pTsPayload, tsPayloadSize);
        _currentOffset = tsPayloadSize;
    } else {
        // append payload data
        assert(_bufferSize >= _currentOffset + tsPayloadSize);
        memcpy(_pCurrentData + _currentOffset, pTsPayload, tsPayloadSize);
        _currentOffset += tsPayloadSize;
    }
}

void PESStream::_WrapUpLastPacket() {
    if(_pCurrentData != nullptr){
        if(_pCurrentHeader->PES_packet_length > 0){
            if(_pCurrentHeader->PES_packet_length + 6 != _currentOffset){
                std::cin.get();
            }
        }
        _packets.push_back(ts2raw::utils::make_unique<PESPacket>(_pCurrentData, _currentOffset, *_pCurrentHeader));
        // how to pass ownership to avoid copy?!?
        delete [] _pCurrentData;
        delete [] _pCurrentHeader;
        _pCurrentData = nullptr;
        _pCurrentHeader = nullptr;
        _currentOffset = 0;
    }
}

void PESStream::Unpack(const std::string& aOutputFilename){
    _WrapUpLastPacket();
    std::ofstream out(aOutputFilename.c_str(), std::ios::out | std::ios::binary);
    if(out.is_open()) {
        for(auto& pPacket : _packets){
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

} //namespace ts2raw
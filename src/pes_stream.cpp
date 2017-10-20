#include "pes_stream.h"
#include "utils.h"

#include <fstream>

// TODO remove
#include <iostream>

using utils::Endianness;
using utils::TSException;

namespace ts2raw {

const int KMaxBufferSize = 1024 * 1024 * 10; // << 10 MB

void PESStream::AddTSPacket(TSPacket& aTsPacket) {
    // is it first packet?
    if(_pid == -1) {
        _pid = aTsPacket.GetPid();
    }

    int tsPayloadSize = 0;
    const unsigned char* pTsPayload = aTsPacket.GetPayload(tsPayloadSize);

    // TODO:
    // last package does not get created
    // PES packet len is 0 for video... :()
    if(aTsPacket.IsPayloadUnitStartIndicatorSet()) {
        // new packet
        // anything already here?
        _WrapUpLastPacket();

        // read pes header
        _pCurrentHeader = new pes_header_t(pTsPayload, tsPayloadSize);

        if(_streamId == -1) {
            _streamId = _pCurrentHeader->stream_id;
        }

        if(_pCurrentHeader->packet_start_code_prefix != 0x01) {
            throw TSException("Inavlid PES packet");
        }

        // TODO: what is pes package size here?
        _allocated = 0;
        if(_pCurrentHeader->PES_packet_length > 0){
            _allocated = _pCurrentHeader->PES_packet_length + 8; // << PES_packet_length is the remainder of the length of th packet
        } else {
            _allocated = KMaxBufferSize;
        }
        _pCurrentData = new unsigned char[_allocated];

        memcpy(_pCurrentData, pTsPayload, tsPayloadSize);
        _currentOffset = tsPayloadSize;
    } else {
        // append payload data
        memcpy(_pCurrentData + _currentOffset, pTsPayload, tsPayloadSize);
        _currentOffset += tsPayloadSize;
    }
}

void PESStream::_WrapUpLastPacket() {
    if(_pCurrentData != nullptr){
        _packets.push_back(utils::make_unique<PESPacket>(_pCurrentData, _currentOffset, *_pCurrentHeader));
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
    return _streamId == 224;
}

bool PESStream::IsAudio() const {
    return _streamId == 192;
}

} //namespace ts2raw
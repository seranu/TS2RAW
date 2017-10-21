#ifndef _PES_STREAM_H_
#define _PES_STREAM_H_

#include <vector>
#include <memory>

#include "pes_packet.h"
#include "ts_packet.h"

namespace ts2raw{

// class used to create PES packets from TS inputs
// TS Packets contain fragmented PES packets
// PES Stream can be used to reconstruct the PES packets
class PESStream{
public:
    PESStream() : 
        _pid(-1), 
        _currentOffset(0), 
        _pCurrentData(nullptr), 
        _pCurrentHeader(nullptr), 
        _streamId(-1),
        _bufferSize(0) {}

    PESStream(int aPid) : 
        _pid(aPid), 
        _currentOffset(0), 
        _pCurrentData(nullptr), 
        _pCurrentHeader(nullptr), 
        _streamId(-1),
        _bufferSize(0) {}

    PESStream(const PESStream&) = delete;
    PESStream& operator=(const PESStream&) = delete;

    // adds a TS packet to the PES Stream
    // the payload of the TS packet will be used to
    // recontruct the PES packet
    // the TS packet must contain PES packet data
    //   <aTsPacket> input TS packet
    void AddTSPacket(TSPacket& aTsPacket);

    // writes gathered PES packets payloads to an output file
    //   <aOutputFilename> output file path
    void Unpack(const std::string& aOutputFilename);

    // returns true if the PES stream holds PES packets denoting a video stream
    bool IsVideo() const;

    // returns true if the PES stream holds PES packets denoting an audio stream
    bool IsAudio() const;

private:
    int _pid;
    int _currentOffset;
    unsigned char* _pCurrentData;
    pes_header_t* _pCurrentHeader;
    std::vector< std::unique_ptr<PESPacket> > _packets;
    int _streamId;
    int _bufferSize;

    // creates a PES packet from gathered data
    // resets internal buffers
    void _WrapUpLastPacket();
}; 

}

#endif //_PES_STREAM_H_
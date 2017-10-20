#ifndef _PES_STREAM_H_
#define _PES_STREAM_H_

#include <vector>
#include <memory>

#include "pes_packet.h"
#include "ts_packet.h"

namespace ts2raw{

class PESStream{
public:

    PESStream() : 
        _pid(-1), 
        _currentOffset(0), 
        _pCurrentData(nullptr), 
        _pCurrentHeader(nullptr), 
        _streamId(-1),
        _allocated(0) {}

    PESStream(int aPid) : 
        _pid(aPid), 
        _currentOffset(0), 
        _pCurrentData(nullptr), 
        _pCurrentHeader(nullptr), 
        _streamId(-1),
        _allocated(0) {}

    PESStream(const PESStream&) = delete;
    PESStream& operator=(const PESStream&) = delete;

    void AddTSPacket(TSPacket& aTsPacket);
    void Unpack(const std::string& aOutputFilename);

    bool IsVideo() const;
    bool IsAudio() const;

private:
    int _pid;
    int _currentOffset;
    unsigned char* _pCurrentData;
    pes_header_t* _pCurrentHeader;
    std::vector< std::unique_ptr<PESPacket> > _packets;
    int _streamId;
    int _allocated;
    void _WrapUpLastPacket();
}; 

}

#endif //_PES_STREAM_H_
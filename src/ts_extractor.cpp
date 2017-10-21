#include "ts_extractor.h"
#include "transport_stream_reader.h"
#include "pes_stream.h"

#include <map>
#include <iostream>

namespace ts2raw {

// determines whether a new TS package contains PES data
static bool IsPESPacket(
    const TSPacket& aTsPacket, 
    const std::map<int, PESStream>& aStreamMap
){
    bool result = false;
    if(aTsPacket.IsPayloadUnitStartIndicatorSet()) {
        // ts packet containing start of packet in payload
        int size = 0;
        const unsigned char* payload = aTsPacket.GetPayload(size);
        // check by header
        if(PESPacket::IsPESPacket(payload, size)) {
            result = true;
        }
    } else {
        // continuation of already started packet in ts payload
        // check if pid is already in streamMap
        if(aStreamMap.find(aTsPacket.GetPid()) != aStreamMap.end()) {
            result = true;
        }
    }   
    return result;
}

int TSExtractor::Extract(
    const std::string& aInputFilename,
    const std::string& aOutputVideoFilename,
    const std::string& aOutputAudioFilename
) {
    TransportStreamReader tsReader(aInputFilename);
    std::map<int, PESStream> streamMap;

    std::unique_ptr<TSPacket> pPacket = std::move(tsReader.NextPacket());

    while(pPacket){
        if(IsPESPacket(*pPacket, streamMap)) {
            streamMap[pPacket->GetPid()].AddTSPacket(*pPacket);
        }
        pPacket = std::move(tsReader.NextPacket());
    }

    for(auto& mapPair : streamMap){
        auto& pesStream = mapPair.second;
        if(pesStream.IsVideo()){
            pesStream.Unpack(aOutputVideoFilename);
        }

        if(pesStream.IsAudio()){
            pesStream.Unpack(aOutputAudioFilename);
        }
    }
    
    return 0;
}

} //namespace ts2raw
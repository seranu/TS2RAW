#include "ts_extractor.h"
#include "transport_stream_reader.h"
#include "pes_stream.h"

#include <map>

namespace ts2raw {

int TSExtractor::Extract(
    const std::string& aInputFilename,
    const std::string& aOutputVideoFilename,
    const std::string& aOutputAudioFilename
) {
    TransportStreamReader tsReader(aInputFilename);
    std::map<int, PESStream> streamMap;

    std::unique_ptr<TSPacket> pPacket = std::move(tsReader.NextPacket());

    while(pPacket){
        // int size = 0;
        // const unsigned char* payload = pPacket->GetPayload(size);
        // if(PESPacket::IsPESPacket(payload, size)){
        if(pPacket->GetPid() != 0 && pPacket->GetPid() != 32) {
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
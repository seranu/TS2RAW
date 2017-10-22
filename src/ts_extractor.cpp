#include "ts_extractor.h"
#include "pes_stream.h"
#include "ts_reader.h"

#include <iostream>
#include <map>

#define NULL_PACKET_PID 0x1FFF

namespace ts2raw {

// determines whether a new TS packet contains PES data
static bool IsPESPacket(const TSPacket& aTsPacket,
                        const std::map<int, PESStream>& aStreamMap) {
  bool result = false;
  if (aTsPacket.IsPayloadUnitStartIndicatorSet()) {
    // ts packet containing start of packet in payload
    int size = 0;
    const unsigned char* payload = aTsPacket.GetPayload(size);
    // check by header
    if (PESPacket::IsPESPacket(payload, size)) {
      result = true;
    }
  } else {
    // continuation of already started packet in ts payload
    // check if pid is already in streamMap
    if (aStreamMap.find(aTsPacket.GetPid()) != aStreamMap.end()) {
      result = true;
    }
  }
  return result;
}

void TSExtractor::Extract(const std::string& aInputFilename,
                          const std::string& aOutputVideoFilename,
                          const std::string& aOutputAudioFilename) {
  TSReader tsReader(aInputFilename);
  std::map<int, PESStream> streamMap;

  // go through all packets in the file
  for (std::unique_ptr<TSPacket> pPacket = tsReader.NextPacket(); pPacket;
       pPacket = tsReader.NextPacket()) {
    // drop null packets
    if (pPacket->GetPid() == NULL_PACKET_PID) {
      continue;
    }

    // only interested in PES packets
    if (IsPESPacket(*pPacket, streamMap)) {
      streamMap[pPacket->GetPid()].AddTSPacket(*pPacket);
    }
  }

  // write PES packets to coresponding output files
  for (auto& mapPair : streamMap) {
    auto& pesStream = mapPair.second;
    if (pesStream.IsVideo()) {
      pesStream.Unpack(aOutputVideoFilename);
    }

    if (pesStream.IsAudio()) {
      pesStream.Unpack(aOutputAudioFilename);
    }
  }
}

}  // namespace ts2raw
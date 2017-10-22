#ifndef _TRANSPORT_STREAM_REDER_H_
#define _TRANSPORT_STREAM_REDER_H_

#include <fstream>
#include <memory>
#include <string>

#include "ts_packet.h"

namespace ts2raw {

// class used to read TSPackets from an input .ts file
class TSReader {
 public:
  // constructor
  //   <inputFilename> file path of input ts file
  TSReader(const std::string& inputFilename);

  TSReader(const TSReader&) = delete;
  TSReader& operator=(const TSReader&) = delete;

  // returns the next TSPacket in the file
  std::unique_ptr<TSPacket> NextPacket();

 private:
  std::ifstream _inputFile;
};

}  // namespace ts2raw

#endif  //_TRANSPORT_STREAM_REDER_H_
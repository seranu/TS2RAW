#ifndef _TRANSPORT_STREAM_REDER_H_
#define _TRANSPORT_STREAM_REDER_H_

#include <string>
#include <memory>
#include <fstream>

#include "ts_packet.h"

namespace ts2raw {

class TransportStreamReader{
public:
    TransportStreamReader(const std::string& inputFilename);

    std::unique_ptr<TSPacket> NextPacket();
private:
    std::ifstream _inputFile;
    std::string _inputFilename;

    void _lazyOpen();
};

}

#endif //_TRANSPORT_STREAM_REDER_H_
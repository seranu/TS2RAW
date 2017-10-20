#include "transport_stream_reader.h"
#include "utils.h"

#include <cassert>

using utils::TSException;

namespace ts2raw {

TransportStreamReader::TransportStreamReader(const std::string& inputFilename) 
    : _inputFile(inputFilename.c_str(), std::ios::in | std::ios::binary) {}
    

void TransportStreamReader::_lazyOpen() {
}

std::unique_ptr<TSPacket> TransportStreamReader::NextPacket() {
    if(!_inputFile.is_open()) {
        throw TSException("Input file is not opened.");
    }

    unsigned char buffer[188];
    if (!_inputFile.eof()) {
        _inputFile.read(reinterpret_cast<char*>(buffer), KStreamPacketSize);

        if (_inputFile.gcount() == KStreamPacketSize) {
            return utils::make_unique<TSPacket>(buffer, KStreamPacketSize);
        } else {
            // end of file
            assert(_inputFile.gcount() == 0);
        }
    }
    return nullptr;

}

}
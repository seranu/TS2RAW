#include "ts_reader.h"
#include "utils.h"

#include <cassert>

using ts2raw::utils::TSException;

namespace ts2raw {

TransportStreamReader::TransportStreamReader(const std::string& inputFilename) 
    : _inputFile(inputFilename.c_str(), std::ios::in | std::ios::binary) {}
    
std::unique_ptr<TSPacket> TransportStreamReader::NextPacket() {
    if(!_inputFile.is_open()) {
        throw TSException("Input file is not opened.");
    }

    unsigned char buffer[188];
    if (!_inputFile.eof()) {
        _inputFile.read(reinterpret_cast<char*>(buffer), KStreamPacketSize);

        if (_inputFile.gcount() == KStreamPacketSize) {
            return ts2raw::utils::make_unique<TSPacket>(buffer, KStreamPacketSize);
        } else {
            // end of file
            assert(_inputFile.gcount() == 0);
        }
    }
    return nullptr;

}

}
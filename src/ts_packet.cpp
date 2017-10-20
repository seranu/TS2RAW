#include "ts_packet.h"
#include "utils.h"

#include <cassert>

// TODO: remove
#include <iostream>
using namespace std;


using utils::Endianness;
using utils::TSException;

namespace ts2raw {

stream_packet_header_t::stream_packet_header(int32_t aInteger) {
    /*
    MPEG transport stream HEADER LAYOUT:
    [ 0, 0, 0, 0, 0, 0, 0, 0 ], [ 1, 2, 3, 4, 4, 4, 4, 4 ], [ 4, 4, 4, 4, 4, 4, 4, 4 ] [ 5, 5, 6, 6, 7, 7, 7, 7 ]

    0 - Sync byte
    1 - Transport Error Indicator (TEI)
    2 - Payload Unit Start Indicator (PUSI)
    3 - Transport Priority
    4 - PID
    5 - Transport Scrambling Control (TSC)
    6 - Adaptation field control
    7 - Continuity counter
    */

    sync_byte = static_cast<uint8_t>((aInteger & 0xFF000000) >> 24);
    transport_error_indicator = ((aInteger & 0x800000) != 0);
    payload_unit_start_indicator = ((aInteger & 0x400000) != 0);
    transport_priority = ((aInteger & 0x200000) != 0);
    pid = static_cast<uint16_t>((aInteger & 0x1fff00) >> 8);
    transport_scrambling_control = static_cast<uint8_t>((aInteger & 0xc0) >> 6);
    adaptation_field_control = static_cast<uint8_t>((aInteger & 0x30) >> 4);
    continuity_counter = static_cast<uint8_t>(aInteger & 0xf);
}

TSPacket::TSPacket(unsigned char* aInput, int size) 
: _header(0),
  _payload(nullptr) {
    assert(size == KStreamPacketSize);

    int offset = KStreamHeaderSize;

    memcpy(_data, aInput, KStreamPacketSize);
    _header = stream_packet_header_t(utils::ReadUInt32(_data, Endianness::BigEndian));

    if(_header.sync_byte != 'G') {
        throw TSException("Invalid input data");
    } // check real value

    // adaptation header exists
	if(_header.adaptation_field_control == 2 ||  // << 10
	_header.adaptation_field_control == 3)  {    // << 11
        uint8_t adaptationHeaderLen = utils::ReadUInt8(_data + offset);
        // if you ever have doubts about +1, look here
        // this +1 is for the UInt8 you just read
		offset += adaptationHeaderLen + 1;
		//cout << "Adaptation header; moving " << static_cast<int>(adaptationHeaderLen) << "\n";
	}

	// payload exists
	if(_header.adaptation_field_control == 1 || // << 01
    _header.adaptation_field_control == 3) {    // << 11
        _payload = _data + offset;
        _payloadSize = KStreamPacketSize - offset;
	}
}

}
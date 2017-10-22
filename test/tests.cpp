#include <bitset>
#include <cassert>
#include <functional>
#include <iostream>
#include <vector>
#include "ts_packet.h"
#include "ts_reader.h"
#include "pes_stream.h"
#include "utils.h"

#define TEST_SUCCESS 0
#define EXPECTATION_FAILED 1
#define TEST_START cout << "Testing " << __FUNCTION__ << " ...";
#define TEST_END(res)   \
  if (res) {            \
    cout << "FAILED\n"; \
  } else {              \
    cout << "PASSED\n"; \
  }                     \
  return res;
#define ASSERT_TRUE(cond)                                                   \
  if (!(cond)) {                                                            \
    cout << "Unit test " << __FUNCTION__ << " failed at line: " << __LINE__ \
         << "\n";                                                           \
    TEST_END(1);                                                            \
  }

#define EXPECT_THROW_BEGIN \
  int thrown = 0;          \
  try {

#define EXPECT_THROW_END(exceptionType) \
  }                                     \
  catch (const exceptionType& ex) {     \
    thrown = 1;                         \
  }                                     \
  if (!thrown) {                        \
    ASSERT_TRUE(false)                  \
  }
#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))
#define ASSERT_EQ(left, right) ASSERT_TRUE((left) == (right))
#define INVALID_TS_FILE_PATH "no_file.ts"
// relative to build directory
#define VALID_TS_FILE_PATH "../sample/test.ts"
#define VALID_TS_WITH_PES_FILE_PATH "../sample/pes_packet.ts"

using namespace std;
using namespace ts2raw;
using namespace ts2raw::utils;

int TestUtils_01_ReadUInt8_01() {
  TEST_START
  const unsigned char buffer[] = {0x01, 0x11, 0xFF, 0xAB};
  int offset = 0;
  ASSERT_EQ(ReadUInt8(buffer), 1);
  ASSERT_EQ(ReadUInt8(buffer + 1), 17);
  ASSERT_EQ(ReadUInt8(buffer + 2), 255);
  ASSERT_EQ(ReadUInt8(buffer + 3), 171);
  TEST_END(TEST_SUCCESS);
}

int TestUtils_01_ReadUInt16_BE_01() {
  TEST_START
  const unsigned char buffer[] = {0x01, 0x11, 0xFF, 0xAB};
  ASSERT_EQ(ReadUInt16(buffer, Endianness::BigEndian), 273);
  ASSERT_EQ(ReadUInt16(buffer + 1, Endianness::BigEndian), 4607);
  ASSERT_EQ(ReadUInt16(buffer + 2, Endianness::BigEndian), 65451);
  TEST_END(TEST_SUCCESS);
}

int TestUtils_01_ReadUInt32_BE_01() {
  TEST_START
  const unsigned char buffer[] = {0x01, 0x11, 0xFF, 0xAB,
                                  0xCB, 0xBD, 0x9A, 0xF1};
  ASSERT_EQ(ReadUInt32(buffer, Endianness::BigEndian), 17956779);
  ASSERT_EQ(ReadUInt32(buffer + 1, Endianness::BigEndian), 301968331);
  ASSERT_EQ(ReadUInt32(buffer + 2, Endianness::BigEndian), 4289448893);
  ASSERT_EQ(ReadUInt32(buffer + 3, Endianness::BigEndian), 2882256282);
  ASSERT_EQ(ReadUInt32(buffer + 4, Endianness::BigEndian), 3418200817);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_01_sync_byte_01() {
  TEST_START
  bitset<32> number("11111111000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.sync_byte, 0xFF);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_02_sync_byte_02() {
  TEST_START
  bitset<32> number("11100111000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.sync_byte, 0xE7);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_03_sync_byte_03() {
  TEST_START
  bitset<32> number("01101100000100010010010000010010");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.sync_byte, 0x6C);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_04_transport_error_indicator_01() {
  TEST_START
  bitset<32> number("00000000100000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_TRUE(header.transport_error_indicator);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_05_transport_error_indicator_02() {
  TEST_START
  bitset<32> number("00000000000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_FALSE(header.transport_error_indicator);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_06_transport_error_indicator_03() {
  TEST_START
  bitset<32> number("00101111100000001010001111100000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_TRUE(header.transport_error_indicator);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_07_payload_unit_start_indicator_01() {
  TEST_START
  bitset<32> number("00000000010000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_TRUE(header.payload_unit_start_indicator);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_08_payload_unit_start_indicator_02() {
  TEST_START
  bitset<32> number("00000000001000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_FALSE(header.payload_unit_start_indicator);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_09_payload_unit_start_indicator_03() {
  TEST_START
  bitset<32> number("00101111010000001010001111100000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_TRUE(header.payload_unit_start_indicator);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_10_transport_priority_01() {
  TEST_START
  bitset<32> number("00000000001000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_TRUE(header.transport_priority);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_11_transport_priority_02() {
  TEST_START
  bitset<32> number("00000000000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_FALSE(header.transport_priority);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_12_transport_priority_03() {
  TEST_START
  bitset<32> number("00101111001001101010001111100111");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_TRUE(header.transport_priority);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_13_PID_01() {
  TEST_START
  bitset<32> number("00000000000110111001110100000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.pid, 0x1B9D);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_14_PID_02() {
  TEST_START
  bitset<32> number("00000000000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.pid, 0);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_15_PID_03() {
  TEST_START
  bitset<32> number("10110001111110111001110110111001");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.pid, 0x1B9D);
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_16_transport_scrambling_control_01() {
  TEST_START
  bitset<32> number("00000000000000000000000011000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.transport_scrambling_control, bitset<2>("11").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_17_transport_scrambling_control_02() {
  TEST_START
  bitset<32> number("00000000000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.transport_scrambling_control, bitset<2>("00").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_18_transport_scrambling_control_03() {
  TEST_START
  bitset<32> number("10110001111110111001110110111001");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.transport_scrambling_control, bitset<2>("10").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_19_adaptation_field_control_01() {
  TEST_START
  bitset<32> number("00000000000000000000000000110000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.adaptation_field_control, bitset<2>("11").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_20_adaptation_field_control_02() {
  TEST_START
  bitset<32> number("00000000000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.adaptation_field_control, bitset<2>("00").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_21_adaptation_field_control_03() {
  TEST_START
  bitset<32> number("10110001111110111001110110111001");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.adaptation_field_control, bitset<2>("11").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_22_continuity_counter_01() {
  TEST_START
  bitset<32> number("00000000000000000000000000001111");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.continuity_counter, bitset<4>("1111").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_23_continuity_counter_02() {
  TEST_START
  bitset<32> number("00000000000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.continuity_counter, bitset<4>("0000").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketHeader_24_continuity_counter_03() {
  TEST_START
  bitset<32> number("10110001111110111001110110111001");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.continuity_counter, bitset<4>("1001").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int TestTSPacketReader_01_NextPacket_01() {
  TEST_START
  EXPECT_THROW_BEGIN
  TSReader tsReader(INVALID_TS_FILE_PATH);
  std::unique_ptr<TSPacket> pack = tsReader.NextPacket();
  EXPECT_THROW_END(TSException);
  TEST_END(TEST_SUCCESS)
}

int TestTSPacketReader_01_NextPacket_02() {
  TEST_START
  TSReader tsReader(VALID_TS_FILE_PATH);
  std::unique_ptr<TSPacket> pack = tsReader.NextPacket();
  ASSERT_EQ(pack->GetPid(), 0x00);
  ASSERT_TRUE(pack->IsPayloadUnitStartIndicatorSet());
  TEST_END(TEST_SUCCESS)
}

int TestPESStream_01_AddPacket_01() {
  TEST_START
  // valid ts file, invalid 
  TSReader tsReader(VALID_TS_FILE_PATH);
  PESStream pesStream;
  auto pPack = tsReader.NextPacket();
  int size = 0;
  const unsigned char* payload = pPack->GetPayload(size);
  ASSERT_FALSE(PESPacket::IsPESPacket(payload, size));
  EXPECT_THROW_BEGIN
  pesStream.AddTSPacket(*pPack);
  EXPECT_THROW_END(TSException)
  TEST_END(TEST_SUCCESS)
}

int TestPESStream_01_AddPacket_02() {
  TEST_START
  TSReader tsReader(VALID_TS_WITH_PES_FILE_PATH);
  PESStream pesStream;
  auto pPack = tsReader.NextPacket();
  int size = 0;
  const unsigned char* payload = pPack->GetPayload(size);
  ASSERT_TRUE(PESPacket::IsPESPacket(payload, size));
  pesStream.AddTSPacket(*pPack);
  TEST_END(TEST_SUCCESS)
}


std::pair<int, int> runTests(
    const std::vector<std::function<int()>>& aTestSuite) {
  int failed = 0;
  int passed = 0;
  for (auto test : aTestSuite) {
    bool pass = (test() == 0);
    if (pass) {
      passed++;
    } else {
      failed++;
    }
  }
  return std::make_pair(passed, failed);
}

void doRunTests() {
  std::vector<std::function<int()>> tests = {
      TestUtils_01_ReadUInt8_01,
      TestUtils_01_ReadUInt16_BE_01,
      TestUtils_01_ReadUInt32_BE_01,
      TestTSPacketHeader_01_sync_byte_01,
      TestTSPacketHeader_02_sync_byte_02,
      TestTSPacketHeader_03_sync_byte_03,
      TestTSPacketHeader_04_transport_error_indicator_01,
      TestTSPacketHeader_05_transport_error_indicator_02,
      TestTSPacketHeader_06_transport_error_indicator_03,
      TestTSPacketHeader_07_payload_unit_start_indicator_01,
      TestTSPacketHeader_08_payload_unit_start_indicator_02,
      TestTSPacketHeader_09_payload_unit_start_indicator_03,
      TestTSPacketHeader_10_transport_priority_01,
      TestTSPacketHeader_11_transport_priority_02,
      TestTSPacketHeader_12_transport_priority_03,
      TestTSPacketHeader_13_PID_01,
      TestTSPacketHeader_14_PID_02,
      TestTSPacketHeader_15_PID_03,
      TestTSPacketHeader_16_transport_scrambling_control_01,
      TestTSPacketHeader_17_transport_scrambling_control_02,
      TestTSPacketHeader_18_transport_scrambling_control_03,
      TestTSPacketHeader_19_adaptation_field_control_01,
      TestTSPacketHeader_20_adaptation_field_control_02,
      TestTSPacketHeader_21_adaptation_field_control_03,
      TestTSPacketHeader_22_continuity_counter_01,
      TestTSPacketHeader_23_continuity_counter_02,
      TestTSPacketHeader_24_continuity_counter_03,
      TestTSPacketReader_01_NextPacket_01,
      TestTSPacketReader_01_NextPacket_02,
      TestPESStream_01_AddPacket_01,
      TestPESStream_01_AddPacket_02};
  std::pair<int, int> results = runTests(tests);
  std::cout << "PASSED: " << results.first << " FAILED: " << results.second
            << "\nPress any key to continue...";
  cin.get();
}

int main() {
  doRunTests();
  return 0;
}
#include "ts_packet.h"
#include "utils.h"
#include <bitset>
#include <cassert>
#include <functional>
#include <iostream>
#include <vector>

#define TEST_START cout << "Testing " << __FUNCTION__ << " ...";
#define TEST_END(res)                                                          \
  if (res) {                                                                   \
    cout << "FAILED\n";                                                        \
  } else {                                                                     \
    cout << "PASSED\n";                                                        \
  }                                                                            \
  return res;
#define ASSERT_TRUE(cond)                                                      \
  if (!(cond)) {                                                               \
    cout << "Unit test " << __FUNCTION__ << " failed at line: " << __LINE__    \
         << "\n";                                                              \
    TEST_END(1);                                                               \
  }
#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))
#define ASSERT_EQ(left, right) ASSERT_TRUE((left) == (right))
#define TEST_SUCCESS 0

using namespace std;
using namespace ts2raw;
using namespace ts2raw::utils;

int testUtils_01_ReadUInt8_01() {
  TEST_START
  const unsigned char buffer[] = {0x01, 0x11, 0xFF, 0xAB};
  int offset = 0;
  ASSERT_EQ(ReadUInt8(buffer), 1);
  ASSERT_EQ(ReadUInt8(buffer + 1), 17);
  ASSERT_EQ(ReadUInt8(buffer + 2), 255);
  ASSERT_EQ(ReadUInt8(buffer + 3), 171);
  TEST_END(TEST_SUCCESS);
}

int testUtils_01_ReadUInt16_BE_01() {
  TEST_START
  const unsigned char buffer[] = {0x01, 0x11, 0xFF, 0xAB};
  ASSERT_EQ(ReadUInt16(buffer, Endianness::BigEndian), 273);
  ASSERT_EQ(ReadUInt16(buffer + 1, Endianness::BigEndian), 4607);
  ASSERT_EQ(ReadUInt16(buffer + 2, Endianness::BigEndian), 65451);
  TEST_END(TEST_SUCCESS);
}

int testUtils_01_ReadUInt32_BE_01() {
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

int testPacketHeader_01_sync_byte_01() {
  TEST_START
  bitset<32> number("11111111000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.sync_byte, 0xFF);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_02_sync_byte_02() {
  TEST_START
  bitset<32> number("11100111000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.sync_byte, 0xE7);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_03_sync_byte_03() {
  TEST_START
  bitset<32> number("01101100000100010010010000010010");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.sync_byte, 0x6C);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_04_transport_error_indicator_01() {
  TEST_START
  bitset<32> number("00000000100000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_TRUE(header.transport_error_indicator);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_05_transport_error_indicator_02() {
  TEST_START
  bitset<32> number("00000000000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_FALSE(header.transport_error_indicator);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_06_transport_error_indicator_03() {
  TEST_START
  bitset<32> number("00101111100000001010001111100000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_TRUE(header.transport_error_indicator);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_07_payload_unit_start_indicator_01() {
  TEST_START
  bitset<32> number("00000000010000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_TRUE(header.payload_unit_start_indicator);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_08_payload_unit_start_indicator_02() {
  TEST_START
  bitset<32> number("00000000001000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_FALSE(header.payload_unit_start_indicator);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_09_payload_unit_start_indicator_03() {
  TEST_START
  bitset<32> number("00101111010000001010001111100000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_TRUE(header.payload_unit_start_indicator);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_10_transport_priority_01() {
  TEST_START
  bitset<32> number("00000000001000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_TRUE(header.transport_priority);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_11_transport_priority_02() {
  TEST_START
  bitset<32> number("00000000000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_FALSE(header.transport_priority);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_12_transport_priority_03() {
  TEST_START
  bitset<32> number("00101111001001101010001111100111");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_TRUE(header.transport_priority);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_13_PID_01() {
  TEST_START
  bitset<32> number("00000000000110111001110100000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.pid, 0x1B9D);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_14_PID_02() {
  TEST_START
  bitset<32> number("00000000000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.pid, 0);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_15_PID_03() {
  TEST_START
  bitset<32> number("10110001111110111001110110111001");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.pid, 0x1B9D);
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_16_transport_scrambling_control_01() {
  TEST_START
  bitset<32> number("00000000000000000000000011000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.transport_scrambling_control, bitset<2>("11").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_17_transport_scrambling_control_02() {
  TEST_START
  bitset<32> number("00000000000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.transport_scrambling_control, bitset<2>("00").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_18_transport_scrambling_control_03() {
  TEST_START
  bitset<32> number("10110001111110111001110110111001");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.transport_scrambling_control, bitset<2>("10").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_19_adaptation_field_control_01() {
  TEST_START
  bitset<32> number("00000000000000000000000000110000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.adaptation_field_control, bitset<2>("11").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_20_adaptation_field_control_02() {
  TEST_START
  bitset<32> number("00000000000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.adaptation_field_control, bitset<2>("00").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_21_adaptation_field_control_03() {
  TEST_START
  bitset<32> number("10110001111110111001110110111001");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.adaptation_field_control, bitset<2>("11").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_22_continuity_counter_01() {
  TEST_START
  bitset<32> number("00000000000000000000000000001111");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.continuity_counter, bitset<4>("1111").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_23_continuity_counter_02() {
  TEST_START
  bitset<32> number("00000000000000000000000000000000");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.continuity_counter, bitset<4>("0000").to_ulong());
  TEST_END(TEST_SUCCESS);
}

int testPacketHeader_24_continuity_counter_03() {
  TEST_START
  bitset<32> number("10110001111110111001110110111001");
  stream_packet_header_t header(number.to_ulong());
  ASSERT_EQ(header.continuity_counter, bitset<4>("1001").to_ulong());
  TEST_END(TEST_SUCCESS);
}

std::pair<int, int>
runTests(const std::vector<std::function<int()>> &aTestSuite) {
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
      testUtils_01_ReadUInt8_01,
      testUtils_01_ReadUInt16_BE_01,
      testUtils_01_ReadUInt32_BE_01,
      testPacketHeader_01_sync_byte_01,
      testPacketHeader_02_sync_byte_02,
      testPacketHeader_03_sync_byte_03,
      testPacketHeader_04_transport_error_indicator_01,
      testPacketHeader_05_transport_error_indicator_02,
      testPacketHeader_06_transport_error_indicator_03,
      testPacketHeader_07_payload_unit_start_indicator_01,
      testPacketHeader_08_payload_unit_start_indicator_02,
      testPacketHeader_09_payload_unit_start_indicator_03,
      testPacketHeader_10_transport_priority_01,
      testPacketHeader_11_transport_priority_02,
      testPacketHeader_12_transport_priority_03,
      testPacketHeader_13_PID_01,
      testPacketHeader_14_PID_02,
      testPacketHeader_15_PID_03,
      testPacketHeader_16_transport_scrambling_control_01,
      testPacketHeader_17_transport_scrambling_control_02,
      testPacketHeader_18_transport_scrambling_control_03,
      testPacketHeader_19_adaptation_field_control_01,
      testPacketHeader_20_adaptation_field_control_02,
      testPacketHeader_21_adaptation_field_control_03,
      testPacketHeader_22_continuity_counter_01,
      testPacketHeader_23_continuity_counter_02,
      testPacketHeader_24_continuity_counter_03};
  std::pair<int, int> results = runTests(tests);
  std::cout << "PASSED: " << results.first << " FAILED: " << results.second
            << "\nPress any key to continue...";
  cin.get();
}

int main() {
  doRunTests();
  return 0;
}
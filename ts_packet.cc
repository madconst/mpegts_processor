#include <vector>
#include <algorithm>
#include <iterator>
#include <boost/cstdint.hpp>

#include <iostream>
#include <iterator>
#include <algorithm>

#include "ts_packet.h"
#include "bitfield.hpp"

using std::vector;
using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

TsPacket::TsPacket(unsigned char* data) // must be exactly 188 bytes long
{
    parsePacket(data);
}
TsPacket::TsPacket(const array<unsigned char, 188>& data)
{
    parsePacket(data.data());
}
void TsPacket::parsePacket(const unsigned char* data)
{
    parseHeader(data);
    unsigned int adf_length = 0;

    if (header.adaptation_field_exists) {
        parseAdaptationField(&data[4]);
        adf_length = adaptation_field.base.length;
    }

    // Construct payload:
    if (header.contains_payload) {
        if (adf_length + 5 >= 188) {
            throw std::string("Adaptation field exceeded packet length");
        }
        payload_chunk = vector<unsigned char>(&data[5] + adf_length, &data[188]);
    }
}
void TsPacket::parseHeader(const unsigned char* data)
{
    std::copy(data, data + 4, header.data);
}
void TsPacket::parseAdaptationField(const unsigned char* data)
{
    std::copy(data, data + 2, adaptation_field.base.data);

    unsigned int index = 2;

    if (adaptation_field.base.pcr_flag) {
        std::copy(data + index, data + index + 6, adaptation_field.pcr.data);
        index += 6;
    }

    if (adaptation_field.base.opcr_flag) {
        std::copy(data + index, data + index + 6, adaptation_field.pcr.data);
        index += 6;
    }

    adaptation_field.splice_countdown = data[index];
}

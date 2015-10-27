#ifndef TS_PACKET_H_
#define TS_PACKET_H_

#include <vector>
#include <tr1/array>
#include <boost/cstdint.hpp>

#include "ts_header.h"
#include "ts_adaptation_field.h"

using std::vector;
using std::tr1::array;

class TsPacket
{
public:
    TsHeader header;
    TsPacket(unsigned char* data);
    TsPacket(const array<unsigned char, 188>& data);
    void parsePacket(const unsigned char* data);
    TsAdaptationField adaptation_field;
    vector<unsigned char> payload_chunk;
    // Array<unsigned char, 188> serialize() const; ?
private:
    void parseHeader(const unsigned char* data);
    void parseAdaptationField(const unsigned char* data);
};

#endif
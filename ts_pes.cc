#include <string>
#include "ts_pes.h"

using std::vector;
using std::string;

TsPES& TsPES::operator<<(const TsPacket& packet)
{
    if (!isEmpty()) {
        TsPacket prev_packet = pes.back();
        // Check continuity counter:
        if (((prev_packet.header.continuity_counter + 1) & 0xf) != (packet.header.continuity_counter & 0xf)) {
            throw string("Continuity error");
        }
        // Check pid matching:
        if (prev_packet.header.pid != packet.header.pid) {
            throw string("PID mismatch");
        }
        // Make sure it's not another payload_unit_start:
        if (packet.header.payload_unit_start == 1 ) {
            throw string("Can't add payload_unit_start to a non-empty PES");
        }
    }

    if (isEmpty() && packet.header.payload_unit_start != 1) {
        throw string("PES must start with payload_unit_start");
    }

    pes.push_back(packet);

    return *this;
}
vector<unsigned char> TsPES::getPayload() const
{
    vector<unsigned char> payload;
    for (vector<TsPacket>::const_iterator it = pes.begin(); it != pes.end(); ++it) {
        payload.insert(
            payload.end(), 
            it->payload_chunk.begin(), 
            it->payload_chunk.end()
        );
    }
    return payload;
}
size_t TsPES::getPayloadSize() const
{
    size_t size = 0;
    for (vector<TsPacket>::const_iterator it = pes.begin(); it != pes.end(); ++it) {
        size += it->payload_chunk.size();
    }
    return size;
}
bool TsPES::isEmpty() const
{
    return pes.empty();
}
unsigned int TsPES::getPid() const
{
    if (pes.size() == 0) {
        throw std::string("Empty PES has no PID");
    }
    return pes[0].header.pid;
}
unsigned int TsPES::getNPackets() const
{
    return pes.size();
}

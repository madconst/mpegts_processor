#ifndef TS_PES_H_
#define TS_PES_H_

#include <vector>
#include "ts_packet.h"

using std::vector;
using std::string;

class TsPES
{
public:
    TsPES& operator<<(const TsPacket& packet);
    vector<unsigned char> getPayload() const;
	size_t getPayloadSize() const;
    bool isEmpty() const;
    unsigned int getPid() const;
    unsigned int getNPackets() const;
private:
    unsigned int pid;
    vector<TsPacket> pes;
};

#endif
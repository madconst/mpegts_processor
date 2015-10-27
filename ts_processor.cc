#include <iostream>
#include <algorithm>
#include <string>

#include "ts_packet.h"
#include "ts_processor.h"

using std::copy;
using std::string;

TsProcessor::TsProcessor() : 
	data_buffer(188*2+1),
	sync(false) {};

TsProcessor::~TsProcessor() {}

TsProcessor& TsProcessor::operator<<(const unsigned char byte)
{
    ++stream_counters.nbytes;
    data_buffer.push_back(byte);

    if (data_buffer.size() == 188*2+1 &&
        	data_buffer[0] == 0x47 &&
        	data_buffer[188] == 0x47 &&
        	byte == 0x47) {
    	if(sync == false) {
    		sync = true;
    		gotSync();
    	}
        try {
            fetchPacket();
        } catch(string e) {
            std::cout << "Cought exception: " << e << std::endl;
        }
    }

    if (data_buffer.size() == 188*2+1) {
        ++stream_counters.nbytes_lost;
    	if(sync == true) {
    		sync = false;
    		lostSync();
    	}
    }

    return *this;
}
void TsProcessor::flush()
{
    // Read the data remaining in the buffer (188*2 bytes):
    while (data_buffer.size() >= 188 && data_buffer[0] == 0x47) {
        ++stream_counters.npackets;
        fetchPacket();
    }
}
void TsProcessor::fetchPacket()
{
    array<unsigned char, 188> data;
    for (unsigned int i = 0; i < 188; ++i) {
        data[i] = data_buffer[0];
        data_buffer.pop_front();
    }
    processPacket(data);
}
void TsProcessor::processPacket(const array<unsigned char, 188>& data)
{
    TsPacket packet(data);
    ++stream_counters.npackets;
    ++pid_counters[packet.header.pid].npackets;
}
void TsProcessor::gotSync() const
{
	std::cout << "Got sync" << std::endl;
}
void TsProcessor::lostSync() const
{
	++stream_counters.nsync_lost;
}

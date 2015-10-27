#ifndef TS_PROCESSOR_H_
#define TS_PROCESSOR_H_

#include <map>
#include <fstream>
#include <tr1/array>
#include <boost/circular_buffer.hpp>

using std::map;
using boost::circular_buffer;
using std::tr1::array;
using std::ofstream;

struct StreamCounters {
	StreamCounters() : 
		npackets(0), 
		nbytes(0), 
		npackets_lost(0), 
		nbytes_lost(0),
		nsync_lost(0) {}
	unsigned int npackets, nbytes,
			     npackets_lost, nbytes_lost,
			     nsync_lost;
};

struct PidCounters {
	PidCounters() : 
		npackets(0), 
		npackets_lost(0) {}
	unsigned int npackets, npackets_lost;
};

class TsProcessor
{
public:
	TsProcessor();
	virtual ~TsProcessor();
	TsProcessor& operator<<(const unsigned char byte);
	void flush();
	mutable StreamCounters stream_counters;
	mutable map<unsigned int, PidCounters> pid_counters;
	friend ofstream& operator<<(ofstream& output, TsProcessor& processor);

private:
	circular_buffer<unsigned char> data_buffer;
	bool sync;
	void fetchPacket();
	virtual void processPacket(const array<unsigned char, 188>& data);
	void gotSync() const;
	void lostSync() const;
};

#endif
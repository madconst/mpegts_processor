#ifndef TS_DEMUX_H_
#define TS_DEMUX_H_

#include <vector>
#include <tr1/array>
#include <boost/crc.hpp>
#include "ts_packet.h"
#include "ts_pat.h"
#include "ts_pmt.h"
#include "ts_pes.h"
#include "ts_processor.h"

using std::tr1::array;
using std::vector;

typedef boost::crc_optimal<32, 0x04C11DB7, 0xFFFFFFFF, 0x00000000, false, false> ts_crc32_t;

struct PesCounters {
    PesCounters() : 
        npackets(0), npackets_lost(0),
        nbytes(0), nbytes_lost(0), ncontinuity_errors(0) {}
    unsigned int npackets, npackets_lost,
                 nbytes, nbytes_lost, ncontinuity_errors;
};

class TsDemuxer: public TsProcessor
{
public:
    TsDemuxer();
    virtual ~TsDemuxer();
    const TsPAT* getPAT() const;
    const TsPMT* getPMT(unsigned int pid) const;
    mutable map<unsigned int, PesCounters> pes_counters;
private:
    virtual void processPacket(const array<unsigned char, 188>& data);
    void assemblePES(const TsPacket& packet);
    virtual void processPES(const TsPES* pes);
    void processPAT(const TsPES* pes);
    void processPMT(const TsPES* pes);
    bool isPmtMapComplete() const;
    TsPAT* pat;
    map<unsigned int, TsPMT*> pmt_map;
    map<unsigned int, TsPES*> pes_map;
};

#endif
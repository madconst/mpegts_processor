#include "ts_demuxer.h"

#include <iomanip>
#include <iostream>
#include <iterator>

using std::map;

TsDemuxer::TsDemuxer() : pat(NULL) {}
TsDemuxer::~TsDemuxer()
{
    if (pat != NULL) {
        delete pat;
    }
    for(map<unsigned int, TsPMT*>::iterator it = pmt_map.begin(); it != pmt_map.end(); ++it) {
        delete it->second;
    }
}
void TsDemuxer::processPacket(const array<unsigned char, 188>& data)
{
    TsPacket packet(data);
    ++stream_counters.npackets;
    ++pid_counters[packet.header.pid].npackets;
    assemblePES(packet);
}
void TsDemuxer::assemblePES(const TsPacket& packet)
{
    map<unsigned int, TsPES*>::iterator pes_it = pes_map.find(packet.header.pid);

    if (packet.header.payload_unit_start) {
        // 1. Extract previous PES (if present) and process payload:
        if (pes_it != pes_map.end()) {
            TsPES* pes = pes_it->second;
            processPES(pes);
            delete pes;
        }

        // 2. Create new PES:
        TsPES *pes = new TsPES();
        *pes << packet;
        // TODO: try to parse PAT and PMT just after one payload_chunk. No need to wait till the next chunk, it may fit in just one.
        pes_map[packet.header.pid] = pes;
    } else {
        if (pes_it == pes_map.end()) {
            ++pid_counters[packet.header.pid].npackets_lost;
            ++stream_counters.npackets_lost;
            return;
        }
        TsPES* pes = pes_it->second;
        try {
            *pes << packet;
        } catch(std::string e) {
            // Assume that's a continuity error
            ++pes_counters[pes->getPid()].ncontinuity_errors;
            // TODO: somehow avoid further processing (mark PES as defective?)
        }
    }
}
void TsDemuxer::processPES(const TsPES* pes)
{
    unsigned int pid = pes->getPid();
    if (pid == 0x0) {
        processPAT(pes);
    }

    if (pat == NULL) {
        stream_counters.npackets_lost += pes->getNPackets();
        ++pes_counters[pes->getPid()].npackets_lost;
        pes_counters[pes->getPid()].nbytes_lost += pes->getPayloadSize();
        return;
    }

    if (pat->isPMT(pid)) {
        processPMT(pes);
    } else {
        // process data PES here
    }

    ++pes_counters[pes->getPid()].npackets;
    pes_counters[pes->getPid()].nbytes += pes->getPayloadSize();
}
void TsDemuxer::processPAT(const TsPES* pes)
{
    TsPAT *pat_ = new TsPAT;
    vector<unsigned char> payload = pes->getPayload();
    copy(payload.begin(), payload.begin() + 8, pat_->data);
    unsigned int pat_length = pat_->section_length + 3; // 3 - offset
    if (pat_length > payload.size()) {
        delete pat_;
        throw string("pat_length overflow");
    }

    unsigned int n_programs = (pat_length - 12) / 4; // pat_length - 8 (program info start) - 4 (crc)
    for (unsigned int i = 0; i < n_programs; ++i)
    {
        unsigned int program_info_index = 8 + i * 4;
        TsPatProgram program;
        copy(payload.begin() + program_info_index, payload.begin() + program_info_index + 4, program.data);
        pat_->programs.push_back(program);
    }

    // Check CRC32:
    ts_crc32_t crc32;
    crc32.process_bytes(&payload[0], pat_->section_length + 3);
    if (crc32.checksum() != 0) {
        delete pat_;
        throw string("PAT checksum failed");
    }
    if (pat) {
        delete pat; // delete old PAT if present
    }
    pat = pat_;
    std::cout << ".";
    std::cout.flush();
}
void TsDemuxer::processPMT(const TsPES* pes)
{
    TsPMT* pmt = new TsPMT;
    vector<unsigned char> payload = pes->getPayload();
    copy(payload.begin(), payload.begin() + 12, pmt->data);

    if (static_cast<size_t>(pmt->program_info_length + 12) > payload.size()) {
        delete pmt;
        throw string("program info length exceeded payload");
    }

    if (pmt->program_info_length) {
        copy(payload.begin() + 12, payload.begin() + 12 + pmt->program_info_length, back_inserter(pmt->descriptor));
    }

    unsigned int program_index = 12 + pmt->program_info_length;

    for (int i = program_index; i < pmt->section_length + 3 - 4;) // 4 - crc32
    {
        TsPmtProgram program;
        copy(payload.begin() + i, payload.begin() + i + 5, program.data);
        pmt->programs.push_back(program);
        i += 5 + program.es_info_length;
        // TODO: check that es_info_length doesn't exceed packet length
    }

    ts_crc32_t crc32;
    crc32.process_bytes(&payload[0], pmt->section_length + 3);
    if (crc32.checksum() != 0) {
        delete pmt;
        throw string("PMT checksum failed");
    }

    const unsigned int pid = pes->getPid();

    map<unsigned int, TsPMT*>::iterator it = pmt_map.find(pid);
    if (it == pmt_map.end()) {
        pmt_map.insert(it, std::pair<unsigned int, TsPMT*>(pid, pmt));
    } else {
        delete it->second;
        it->second = pmt;
    }
}

bool TsDemuxer::isPmtMapComplete() const
{
    if (pat == NULL) {
        return false;
    }
    for(vector<TsPatProgram>::iterator it = pat->programs.begin(); it != pat->programs.end(); ++it) {
        if (pmt_map.find(it->program_map_pid) == pmt_map.end()) {
            return false;
        }
    }
    return true;
}

const TsPAT* TsDemuxer::getPAT() const
{
    return pat;
}
const TsPMT* TsDemuxer::getPMT(unsigned int pid) const
{
    map<unsigned int, TsPMT*>::const_iterator it = pmt_map.find(pid);
    if (it == pmt_map.end()) {
        return NULL;
    } else {
        return it->second;
    }
}

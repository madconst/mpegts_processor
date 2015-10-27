#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <iomanip>
#include <ctime>
#include <map>
#include <csignal>
#include <boost/program_options.hpp>

#include "ts_demuxer.h"

namespace po = boost::program_options;

using std::cout;
using std::endl;
using std::dec;
using std::hex;
using std::setw;

void sigintHandler(int signum)
{
    throw signum;
}

void printPMT(const TsPMT* pmt)
{
    if (pmt == NULL) {
        cout << "PMT missing" << endl;
        return;
    }

    cout << "    table id: " << static_cast<unsigned>(pmt->table_id);
    cout << ", version: " << static_cast<unsigned>(pmt->version_number);
    cout << ",  transport stream id: " << static_cast<unsigned>(pmt->transport_stream_id) << endl;
    
    for (vector<TsPmtProgram>::const_iterator it = pmt->programs.begin(); it != pmt->programs.end(); ++it)
    {
        cout << "    PID: 0x" << hex << static_cast<unsigned>(it->elementary_pid);
        cout << ", stream type: 0x" << static_cast<unsigned>(it->stream_type);
        if (pmt->pcr_pid == it->elementary_pid) {
            cout << ", PCR";
        }
        cout << endl;
    }
}

void printProgramTable(const TsDemuxer& ts_demuxer)
{
    const TsPAT* pat = ts_demuxer.getPAT();

    if (pat == NULL) {
        cout << "PAT missing" << endl;
        return;
    }

    cout << "table id: " << static_cast<unsigned>(pat->table_id);
    cout << ", version: " << static_cast<unsigned>(pat->version_number) << endl;
    for (vector<TsPatProgram>::const_iterator it = pat->programs.begin(); it != pat->programs.end(); ++it)
    {
        cout << dec;
        cout << "  Program #" << static_cast<unsigned>(it->program_number);
        cout << ", PID: 0x" << hex << static_cast<unsigned>(it->program_map_pid);
        cout << endl;
        printPMT(ts_demuxer.getPMT(it->program_map_pid));
    }

}

void printNPesByPID(const map<unsigned int, PesCounters> &pes_counters)
{
    for(map<unsigned int, PesCounters>::const_iterator it = pes_counters.begin(); it != pes_counters.end(); ++it) {
        cout << hex;
        cout << "PID: " << setw(5) << it->first;
        cout << dec;
        cout << ", " << dec << setw(5) << it->second.npackets << " packets processed";
        cout << ", " << dec << setw(10) << it->second.nbytes << " bytes";
        if (it->second.npackets_lost) {
            cout << ", " << dec << setw(5) << it->second.npackets_lost << " packets lost";
        }
        if (it->second.nbytes_lost) {
            cout << ", " << dec << setw(10) << it->second.nbytes_lost << " bytes lost";
        }
        if (it->second.ncontinuity_errors) {
            cout << ", " << dec << setw(10) << it->second.ncontinuity_errors << " continuity errors";
        }        
        cout << endl;
    }
}

void printNPacketsByPID(const map<unsigned int, PidCounters> &pid_counters)
{
    for(map<unsigned int, PidCounters>::const_iterator it = pid_counters.begin(); it != pid_counters.end(); ++it) {
        cout << hex;
        cout << "PID: " << setw(5) << it->first;
        cout << dec;
        cout << ", " << setw(5) << it->second.npackets << " packets processed";
        if (it->second.npackets_lost) {
            cout << ", " << setw(5) << it->second.npackets_lost << " packets lost";
        }
        cout << endl;
    }
}

void printStatistics(TsDemuxer& ts_demuxer)
{
    clock_t ticks = clock();

    cout << endl;
    cout << "Statistics: " << endl;
    cout << "Packets total:\t" << ts_demuxer.stream_counters.npackets << endl;
    cout << "Bytes total:\t" << ts_demuxer.stream_counters.nbytes << endl;
    cout << "Bytes lost:\t" << ts_demuxer.stream_counters.nbytes_lost << endl;
    cout << "Sync errors:\t" << ts_demuxer.stream_counters.nsync_lost << endl;
    cout << "Time elapsed:\t" << (static_cast<float>(ticks)/CLOCKS_PER_SEC) << " sec" << endl;
    cout << endl;

    cout << "TS packets stats by PID: " << endl;
    printNPacketsByPID(ts_demuxer.pid_counters);
    cout << endl;

    cout << "PES packets stats by PID: " << endl;
    printNPesByPID(ts_demuxer.pes_counters);
    cout << endl;

    cout << "Program table: " << endl;
    printProgramTable(ts_demuxer);
}

int main(int argc, char *argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "show help message")
        ("input-file,i", po::value<string>(), "set the input file name");

    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (po::unknown_option e) {
        cout << e.what() << endl;
        return 1;
    } catch (po::invalid_command_line_syntax e) {
        cout << "Invalid command line syntax" << endl;
        return 1;
    }

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    string input_filename;
    if (vm.count("input-file")) {
        input_filename = vm["input-file"].as<string>();
    } else {
        cout << "Please specify input file name. Type --help for help" << endl;
        return 1;
    }

    std::ifstream input(input_filename.c_str(), std::ios::in | std::ios::binary);
    if (input.fail()) {
        cout << "Failed to open the input file" << endl;
        return 1;
    }

    TsDemuxer ts_demuxer;

    std::signal(SIGINT, sigintHandler);

    try {
        for (std::istreambuf_iterator<char> it = std::istreambuf_iterator<char>(input); it != std::istreambuf_iterator<char>(); ++it) {
            ts_demuxer << *it;
        }
    } catch(int signum) {
        if(signum != SIGINT) {
            throw;
        }
        ts_demuxer.flush();
    } catch(std::string e) {
        cout << "Cought exception: " << e << endl;
    }

    printStatistics(ts_demuxer);
    input.close();

    return 0;
}

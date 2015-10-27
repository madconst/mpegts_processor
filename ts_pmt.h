#ifndef TS_PMT_H_
#define TS_PMT_H_

#include <vector>
#include <string>
#include "bitfield.hpp"

struct TsPmtProgram
{
    union {
        unsigned char data[5];
        BitField_BE<0,8> stream_type;
        BitField_BE<11,13> elementary_pid;
        BitField_BE<28,12> es_info_length;
    };
    std::string descriptor;
};

struct TsPMT
{
    union {
        unsigned char data[12];
        BitField_BE<0,8> table_id;
        BitField_BE<8,1> section_syntax_indicator;
        
        BitField_BE<12,12> section_length;
        BitField_BE<24,16> transport_stream_id;
        BitField_BE<42,5> version_number;
        BitField_BE<47,1> current_next_indicator;
        BitField_BE<48,8> section_number;
        BitField_BE<56,8> last_section_number;

        BitField_BE<67,13> pcr_pid;
        BitField_BE<84,12> program_info_length;
    };
    std::vector<unsigned char> descriptor;
    std::vector<TsPmtProgram> programs;

    bool isEmpty();
    bool isPMT(unsigned int pid);
    size_t getNPrograms();
};

#endif